
play
	call waitForKeyRelease		;keyhandler is very fast, so make sure there are currently no keys pressed
	
init					;sandboxing the actual player so we can handle keypresses vs looping quickly
	ld de,ptns
initrp					;init point for row play
	ld hl,-12			;calculate stack restore point
	add hl,sp
	ld (oldSP),hl

	push de
	call resetFX0

	ld hl,(musicData+1)		;load user (drum) sample pointer \ this will become redundant, (usrdrum) can be set on loading song
	ld (usrdrum),hl			;save it to drum table           / and if song is already in mem, it is then also already loaded

	ld a,(looprow)			;set loop point
	;add a,a				;A*4
	;add a,a
	ld l,a
	ld h,0
	add hl,hl			;loop point x4 because each seqrow is 4 bytes
	add hl,hl
	ld de,ptns
	
	ld a,(de)			;while we're at it, check if player is set to start from an empty row (first pattern # = #ff)
	
	add hl,de			;and calculate loop point
	ld (looppoint),hl
	pop de
	
	inc a				;continue "empty row" check
	ret z				;and exit if an "empty" row is found

	call rdnotes1

init0
	call rdnotes0

	jp z,init0			;loop if no key pressed

;*************************************************************************************
rdnotes0				;initialize read song data process
looppoint equ $+1
	ld de,0				;point song data pointer at loop point
					
	ld a,(de)			;check if loop point is still valid - exit with error if not
	inc a
	jp z,lpInvalidErr
	
rdnotes1
	push de				;preserve song data pointer
	xor a
	out (kbd),a
	jr ptnselect

;*************************************************************************************
ptnselect0
	pop hl				;clean stack - PROBABLY BETTER TO USE LD SP,nnnn
	pop hl
	pop hl
	pop hl
;*************************************************************************************
ptnselect				;FIRST, set up ptn sequence pointers and push them (preserve song seq pointer in mem?)
					;SECOND, check if row counter has reached 0
					;THIRD, pop back ptn sequence pointers and read in data
	
	ld a,#11		;7	;initialize ptn position counter - all patterns are 16 rows long
	ld (reptpos),a		;13

	pop de			;10	;restore song data pointer
	
	ld a,(de)		;7	;load ptn# ch1
	cp #ff			;7	;check for end marker
	ret z			;11/5	;exit if found


	ld h,HIGH(ptntab)		;point to pattern position LUT
	add a,a			;4	;A=A*2
	ld l,a			;4
	ld c,(hl)		;7	;seq pointer ch1 to bc
	inc hl			;6	;TODO: in theory inc l should be sufficient?
	ld b,(hl)		;7
	push bc			;11

	inc de			;6	;point to ch3
	ld a,(de)		;7	;load ptn# ch3
	add a,a			;4
	ld l,a			;4
	ld c,(hl)		;7	;seq pointer ch3 to bc
	inc hl			;6
	ld b,(hl)		;7
	push bc			;11
		
	inc de			;6	;point to ch2
	ld a,(de)		;7	;load ptn# ch2
	add a,a			;4
	ld l,a			;4
	ld c,(hl)		;7	;seq pointer ch2 to bc
	inc hl			;6
	ld b,(hl)		;7
	push bc			;6
	
	inc de			;6	;point to fx
	ld a,(de)		;7	;load ptn# fx
	ld hl,fxptntab		;10
	add a,a			;4
	add a,l			;4
	ld l,a			;4
IF MODEL != TI8X || MODEL != TI8XS	;fx ptn table crosses page boundary on 82/83 TODO: conditional should depend on the table crossing the page bound
	jr nc,psskip4		;12/7
	inc h			;4
psskip4
ENDIF
	ld c,(hl)		;7	;seq pointer fx to bc
	inc hl			;6
	ld b,(hl)		;7 54t
	
	inc de			;6	;point to next row
	
	exx			;4
	
	pop bc			;10	;seq pointer ch2 to bc'
	pop de			;10	;seq pointer ch3 to de'
	pop hl			;10	;seq pointer ch1 to hl'
	
	exx			;4
	
	push de			;11	;stack loc. 1 - sng seq pointer
	push bc			;11	;stack loc. 2 - fx ptn pointer
	
	exx			;4
	
	push bc			;11	;stack loc. 3 - ch2 ptn pointer
	push de			;11	;stack loc. 4 - ch3 ptn pointer
	push hl			;11	;stack loc. 5 - ch1 ptn pointer
	
;*************************************************************************************		
rdnotes				;read in next step from song data
					;ch1 - sp, ch2 - de, ch3 - bc, speed - de'
	
	ld hl,reptpos		;10	;update pattern row counter
	dec (hl)		;11
	jr z,ptnselect0		;12/7

rdnotesRP				;entry point for RowPlay	
	ld h,HIGH(NoteTab)	;7	;point to frequency LUT	
	pop bc			;10	;ch1 ptn pointer to bc
	ld a,(bc)		;7
	inc bc			;6	;increment ptn pos pointer
	add a,a			;4
	ld l,a			;4	;set offset in freq.LUT
	
	ld a,(hl)		;7	;lookup lo byte of frequency value
	ld (ch1),a		;13	;and store it to base counter
	inc l			;4
	ld a,(hl)		;7
	ld (ch1+1),a		;13	;and store it to base counter

	pop de			;10	;ch2 ptn pointer to de
	ld a,(de)		;7
	inc de			;6	;increment ptn pos pointer
	add a,a			;4
	ld l,a			;4
	
	ld a,(hl)		;7	;get freq.counter for ch2(!)
	inc l			;4
	ld h,(hl)		;7
	ld l,a			;4
	ld (ch3),hl		;16
	
	exx			;4
	
	ld h,HIGH(NoteTab)	;7	;reset freq.LUT pointer
	pop bc			;10	;ch3 ptn pointer to bc'
	ld a,(bc)		;7
	inc bc			;6
	add a,a			;4
	ld l,a			;4
	
	ld a,(hl)		;7	;get freq.counter for ch3(!)
	inc l			;4
	ld h,(hl)		;7
	ld l,a			;4
	ld (ch2),hl		;16
		
	or h			;4	;deactivate pitch slide and table execution on rest notes, else activate	
	ld i,a			;9	;(de)activate table execution
	jr z,_skip		;12/7
	ld a,#eb		;7	;activate slide (#eb = ex de,hl)
_skip
	ld (slideswitch),a	;13
	
	pop de			;10	;fx ptn pointer to de
	ld a,(de)		;7	;read drum/fx cmd
	
	and %11110000		;7	;check for drum trigger (lower nibble)
 	jp nz,drums		;10
	
	ld a,#ee		;7	;deactivate drum (#ee = xor n)
drx
	ld (noDrum),a		;13
	
	ld a,(de)		;7	;read fx cmd again
	and %00001111		;7	;
	inc de			;6	;point to fx parameter
	jp nz,fxselect		;10
	
fxreturn
	inc de			;6	;point to next row

	push de			;11	;stack 2			
	push bc			;11	;stack 3

	ld bc,0			;10	;BC' is add value for ch2, zero it

xtab equ $+2
	ld ix,ptn00		;14	;pattern pointer for "execute note table" effect

ch2 equ $+1
	ld de,0			;10	;DE' holds the base counter for ch3

	ld iy,0			;14	;IY is the add value for ch3, zero it
	
	exx			;4
	push de			;11	;stack 4
	push bc			;11	;stack 5

drumtrig equ $+1
	ld bc,0			;10

cspeed equ $+2
	ld de,#1000		;10	;speed
	
ch1 equ $+1
	ld sp,#0000		;10	;load base frequency counter val

	ld hl,0			;10	;HL holds "add" value for ch1, zero it

maskD equ $+1				;panning switch for drum ch
	ld a,lp_off			;initialize output mask for drum channel
	ex af,af'

;*************************************************************************************
noXFX
playnote				;synthesize and output current step
					
	ld a,(bc)		;7	;load sample byte
drumswap2
	nop			;4	;daa/cpl/etc
noDrum
dru equ $+1
	add a,0			;7	;add current drum pitch counter - swapped out with xor n (#ee) when not playing a drum (add a,n = #c6)
	ld (dru),a		;13	;and save it as current pitch counter

muteD					;mute switch for drums
	jp nc,waitD		;10	;jp nc = #d2, jp = #c3
	
	ex af,af'		;4	;load output mask for drum channel
	out (link),a		;11	;output drum channel state
					;---- CH2: 100t	

drumswap				;switch for drum mode. inc bc = #03, dec bc = #0b, inc c = #0c, dec c = #0d, nop	
	inc bc			;6	;increment sample data pointer
panD equ $+1
	xor lp_sw		;7	;toggle output mask	
	ret c			;5	;43t, ret never taken

outdr	
	ex af,af'		;4
	add hl,sp		;11	;add current counter to base freq.counter val. ch1 and save result in HL
phaseshift1 equ $+1
	ld a,#80		;7	;set duty
	cp h			;4
mute1					;mute switch for ch1
	sbc a,a			;4
	or lp_off		;7
pan1 equ $+1
	and lp_on		;7

	exx			;4	;back to the normal register set
out1
	out (link),a		;11	;output state ch1
					;----- DRUMS: 75/75t
					
	
	add iy,de		;15	;update counter ch3
phaseshift3 equ $+1			;switch for phase shift/set duty cycle
	ld a,#80		;7	
	cp iyh			;8
mute3
	sbc a,a			;4	;result of this will be either #00 or #ff. for mute, swap #9f (sbc a,a) with #97 (sub a,a)
	or lp_off		;7
pan3 equ $+1
	and lp_on		;7	;and thus we derive the output state
pitchslide equ $+1
	ld hl,0			;10	;switch for pitch slide
	add hl,de		;11
slideswitch
	ex de,hl		;4	;#eb = ex de,hl || nop (when initial DE = 0)
	
ch3 equ $+1				;misnomer, this is ch2
	ld hl,#0000		;10	;and now, same as above but for ch2
out3
	out (link),a		;11
					;---- CH1: 94t
	

	add hl,bc		;11
	ld b,h			;4
	ld c,l			;4	

					;Duty Modulation FX
					;and 0, xor/add = regular mode
					;and n, xor = synced pwm mod  --> 7xx: xx=0 off, xx < #7f fast pwm, xx > #7f synced pwm
					;and n, add = SID/PWM   --> 5xx w/ xx > #80 -> n = xx&#7f (so 581 would be regular SID)
					;add n, add = fastPWM/auto chord	
					
					;<=580: dutyModSwitch1 = [AND N, dutyModSwitch2 = XOR/ADD N,] dutyMod = 0, phaseShift = xx
					;>580: dutyModSwitch1 = [AND N, dutyModSwitch2 = ADD N,] dutyMod = xx&#7f
					
					;=700: dutyModSwitch1 = AND N, dutyModSwitch2 = ADD N, dutyMod = 0, sync on [, phaseShift = #80]
					;<780: dutyModSwitch1 = ADD N, dutyModSwitch2 = ADD N, dutyMod = xx, sync off
					;>=780: dutyModSwitch1 = AND N, dutyModSwitch2 = XOR N, dutyMod = xx - #7f, sync on
					
					;add n, xor = also valid, but for what?
syncSwitch					
	sbc a,a			;4	;supply sync with main osc for duty modulation fx (sub a,a = #97; sbc a,a = #9f)
dutyModSwitch1
dutyMod equ $+1
	and #0			;7	;and n = #e6; add a,n = #c6
dutyModSwitch2
phaseshift2 equ $+1
	add a,#80		;7	;add a,n = #c6; xor n = #ee
	
	ld (phaseshift2),a	;13	
	cp b			;4
mute2
	sbc a,a			;4
	or lp_off		;7
pan2 equ $+1
	and lp_on		;7
out2
	out (link),a		;11
					;---- CH3: 83t
readkeys				;check if a key has been pressed
	in a,(kbd)		;11
	cpl			;4	;COULD IN THEORY OPTIMIZE THIS AWAY
	or a			;4
	jr nz,keyPressed	;12/7	;and exit if necessary

reentry	
	exx			;4

	dec e			;4	;update timer - slightly inefficient, but faster on average than dec de\ld a,d\or e, and gives better sound
	jp nz,playnote		;10
				;100+75+94+83 = 352t

	dec d				;update timer hi-byte
xFX equ $+1
	jp nz,noXFX

oldSP equ $+1
	ld sp,0				;retrieve SP
rowplaySwap equ $+1			;switch for jumping to exitRowplay instead
	jp rdnotes

;*************************************************************************************
noteCut					;note cut effect for ch1
nLength equ $+1
	ld a,0				;update length counter
	dec a
	ld (nLength),a
	jr nz,noXFX			;resume playback if length counter != 0
	ld hl,0				;else, zero freq. counters
	ld sp,hl
nLengthBackup equ $+1
	ld a,0				;reset length counter
	ld (nLength),a
	jp playnote			;resume playback
	
;*************************************************************************************
execTable				;execute note table effect ch3

	ld a,i				;skip table execution on rests
	jr z,playnote
	exx		
	ld h,HIGH(NoteTab)		;point to frequency table
	ld a,(ix+0)			;read note byte
	inc ix				;inc pattern pointer
	add a,a				;fetch new freq.counter value
	ld l,a
	ld e,(hl)
	inc l
	ld d,(hl)
	exx

	jp playnote			;resume playback
	
;*************************************************************************************
keyPressed
	ld (counterSP),sp	;20	;save reload value (to be used after keyhandling)
	ld sp,(oldSP)
	
	push de				;preserve all counters
	push bc
	exx
	push de
	push hl
	push bc
	ex af,af'
	push af

	call keyhand
	
	pop af
	ex af,af'
	
	pop bc				;retrieve all counters
	pop hl
	pop de
	exx
	pop bc
	pop de

counterSP equ $+1
	ld sp,0

	xor a
	out (kbd),a
	
	jp reentry			;and continue playing

;*************************************************************************************
waitD
 	ex af,af'		;4	;load output mask for drum channel
 	out (link),a		;11	;output drum channel state
 					;---- CH2: 100t	
fxswap1
fxswap2 equ $+1
	dw 0			;8	;swap with rlc h (cb 04) for noise/glitch effect, with rlc l (cb 05) for phase effect
	jp outdr		;10
				;15+18+10=43

;*************************************************************************************

drums					;select drum
	ld hl,samples-2			;point to beginning of sample pointer table - 2 (because minimum offset will be +2)
	rra				;divide drum # by 8 to get offset in table (carry is cleared before calling drum select)
	rra
	rra
	add a,l				;add offset to (h)l
	ld l,a
	ld a,(hl)			;fetch drum data pointer
	inc l
	ld h,(hl)
	ld l,a
	ld (drumtrig),hl
	ld a,#c6			;#c6 = ld a,n
	jp drx
	
;*************************************************************************************	
fxselect				;select fx
	dec a				;calculate jump
	add a,a				
	add a,a
	ld (_jump),a
_jump equ $+1
	jr $
	
fxJumpTab
	jp fx1
	nop
	jp fx2
	nop
	jp fx3
	nop
	jp fx4
	nop
	jp fx5
	nop
	jp fx6
	nop
	jp fx7			;fx7
	nop
	jp fx8			;fx8
	nop
	jp fx9			;fx9
	nop
	jp fxA
	nop
	jp fxB
	nop
	jp fxC
	nop
	jp fxD
	nop
	jp fxE
	nop

fxF					;#0f = set speed
	ld a,(de)
	ld l,a
	and #3f
	ld (cspeed),a
	ld a,l
	and #c0
	ld (cspeed-1),a
	jp fxcont

fxB					;#0b = break/ptn loop cmd
	ld a,(de)			;if param=0
	or a
	jr nz,_ptnloop
	ld a,(reptpos)			;break ptn
	cp #10				;ignore cmd if triggered on the first pattern row
	jp nz,ptnselect			;select next ptn if found  ATTN: leaves register set swapped!
	jp fxreturn

_ptnloop
ptnreptpos equ $+1
	ld a,0
	sub 1
	jp z,fxcont0			;if rept.counter is about to reach 0, all loops have been executed -> reset counter and proceed as normal
	jp nc,execPtnRept		;if rept.counter is now -1, initialize ptn loop

setupPtnRept	
	ld a,(de)			;x = # of repeats, y = # of lines to jump back
	and #f
	inc a
	ld h,a
	ld a,(reptpos)
	add a,h
	cp #12
	jp nc,fxcont			;invalidate effect if backjump crosses pattern boundary
	
	ld (xreptPos),a			;TODO: Need to prevent nested Bxy loops. Maybe.
	
	ld a,(de)
	and #f0
	rra
	rra
	rra
	rra
	
execPtnRept	
	ld (ptnreptpos),a
xreptPos equ $+1
	ld a,0
	ld (reptpos),a
	ld a,(de)
	and #f
	inc a				;y=0 means jump back 1 row, because pointers are already incremented
	neg
	exx
	ld l,a				;load to HL and sign-extend
	ld h,#ff
	ex de,hl
	add hl,de			;set ch1 pointer
	ex de,hl
	add hl,bc			;set ch2 pointer
	ld b,h
	ld c,l
	exx
	ld l,a
	ld h,#ff
	ex de,hl
	add hl,de			;*2 because fx ptns take 2 bytes per row
	add hl,de
	ex de,hl
	add hl,bc			;set ch3 pointer
	ld b,h
	ld c,l
	jp fxcont
	
	
fx1					;#01 = set panning
	ld a,(de)

pch1
	rrca
	jp c,setright1
	rrca
	jp c,setleft1
	ex af,af'
	ld a,lp_on
	ld (pan1),a
	ex af,af'
	
pch2
	rrca
	jp c,setright2
	rrca
	jp c,setleft2
	ex af,af'
	ld a,lp_on
	ld (pan2),a
	ex af,af'	

pch3
	rrca
	jp c,setright3
	rrca
	jp c,setleft3
	ex af,af'
	ld a,lp_on
	ld (pan3),a
	ex af,af'
	
pchD
	rrca
	jp c,setrightD
	rrca
	jp c,setleftD
	;ex af,af'
	ld a,lp_sw
	ld (panD),a
	ld a,lp_off
	ld (maskD),a
	;ex af,af'
	jp fxcont
	
fx2					;pitch slide up
	ld a,(de)
	ld (pitchslide),a
	jp fxcont
	
fx3					;pitch slide down
	ld a,(de)
	ld (pitchslide),a
	or a
	jr z,_skip
	ld a,#ff
_skip
	ld (pitchslide+1),a
	jp fxcont	

fx4					;duty cycle ch1
	ld a,(de)
	ld (phaseshift1),a
	cp #81				;if duty < #80, deactivate Axx
	jr c,Askip
	ld a,4				;else, activate noise mode (A01)
	jr Afast
	
fxA					;ch1 "glitch" effect
	ld a,(de)
	or a
	jr z,Askip
	dec a
	ld a,5
	jr nz,Afast

	dec a
Afast
	ld (fxswap2),a
	ld a,#cb
	ld (fxswap1),a
	jp fxcont
Askip
	;xor a
	;ld (fxswap1),a
	;ld (fxswap2),a
	ld hl,0
	ld (fxswap1),hl
	jp fxcont
	

fx5					;duty cycle ch2
	ld a,(de)
	cp #81				;if fx param > #80, switch to pwm sweep mode
	jr nc,_activatePWM
	
	ld (phaseshift2),a		;set duty
	xor a
	ld (dutyMod),a			;reset duty modulator
	;ld a,#c6			;deactivate pwm sweep
	;ld (pwmswitch),a	
	jp fxcont
	
_activatePWM
	;ld a,#ce
	;ld (pwmswitch),a
	and #7f
	ld (dutyMod),a
	jp fxcont
	
fx6					;duty cycle ch3
	ld a,(de)
	ld (phaseshift3),a
	jp fxcont
	
fx7
	ld a,(de)
	or a
	jr nz,_activate
	
	ld (dutyMod),a
	ld a,#e6
	ld (dutyModSwitch1),a
	ld a,#c6
	ld (dutyModSwitch2),a
	ld a,#9f
	ld (syncSwitch),a
	jp fxcont
	
_activate
	cp #80
	jr c,_autochord
	
	sub #7f
	ld (dutyMod),a
	ld a,#e6
	ld (dutyModSwitch1),a
	ld a,#ee
	ld (dutyModSwitch2),a
	ld a,#9f
	ld (syncSwitch),a
	jp fxcont
	
	
_autochord
	ld (dutyMod),a
	ld a,#c6
	ld (dutyModSwitch1),a
	ld (dutyModSwitch2),a
	ld a,#97
	ld (syncSwitch),a
	jp fxcont
	
; 	ld a,(de)
; 	ld (fastpwmswitch),a
; 	jp fxcont	
	
fx8					;execute note table ch2
	ld a,(de)
	;exx
	ld h,HIGH(ptntab)		;point to pattern position LUT
	add a,a				;A=A*2
	jr c,disableExt

	ld l,a			
	ld a,(hl)			;lo-byte pattern pointer
	inc hl			
	ld h,(hl)			;hi-byte pattern pointer
	ld l,a
	ld (xtab),hl
	ld hl,execTable
	ld (xFX),hl
	;exx
	jp fxcont
	
fx9					;ch3 "glitch" effect
	ld a,(de)
	ld (pitchslide+1),a
	jp fxcont



fxC					;note cut ch1
	ld a,(de)
	or a
	jr z,disableExt
	ld (nLength),a
	ld (nLengthBackup),a
	;exx
	ld hl,noteCut
	ld (xFX),hl
	;exx
	jp fxcont

disableExt				;disable extended effects (8xx, Cxx)
	;exx
	ld hl,noXFX
	ld (xFX),hl
	;exx
	jp fxcont	
	
fxD					;set drum mode.
	ld a,(de)
	and #0f				;calculate jump
	add a,a
	add a,a
	ld (_jumpx),a
_jumpx equ $+1
	jr $

_fxDx0
	ld a,0				;nop (default)
	jr _fxDxx
_fxDx1
	ld a,#27			;daa
	jr _fxDxx
_fxDx2
	ld a,#8f			;add a,a
	jr _fxDxx
_fxDx3
	ld a,#1f			;rra
	jr _fxDxx
_fxDx4
	ld a,#2f			;cpl
	jr _fxDxx
_fxDx5
	ld a,#79			;ld a,c
	jr _fxDxx
_fxDx6
	ld a,#89			;add a,c
	jr _fxDxx
_fxDx7
	ld a,#88			;add a,b
	jr _fxDxx
_fxDx8
	ld a,#90			;sub b
	jr _fxDxx
_fxDx9
	ld a,#91			;sub c
	jr _fxDxx
_fxDxa
	ld a,#a0			;and b
	jr _fxDxx
_fxDxb
	ld a,#a1			;and c
	jr _fxDxx
_fxDxc
	ld a,#b0			;or b
	jr _fxDxx
_fxDxd
	ld a,#b1			;or c
	jr _fxDxx
_fxDxe
	ld a,#a8			;xor b
	jr _fxDxx
_fxDxf
	ld a,#a9			;xor c

_fxDxx
	ld (drumswap2),a
	ld a,(de)
	and #70
	cp #41				;if high nibble > 4, ignore
	jp nc,fxcont
	
	ccf				;calculate jump
	rra
	rra
	ld (_jump),a
	
_jump equ $+1
	jr $

_fxD0x
	ld a,#03			;inc bc
	jr _fxDyy	
_fxD1x
	ld a,#0b			;dec bc
	jr _fxDyy	
_fxD2x
	ld a,#0c			;inc c
	jr _fxDyy	
_fxD3x
	ld a,#0d			;dec c
	jr _fxDyy
_fxD4x
	xor a				;nop

_fxDyy
	ld (drumswap),a					
	jp fxcont



fxE					;reset all fx
	ld a,(de)
	
	cp #80				;if fx param >= 0x80, it's a reset fx
	jr c,_extfx

	and #7f
	call z,resetFX			;resetFX always returns with A=0 and Z-flag set	
	dec a
	call z,resetFX1
	dec a
	call z,resetFX2
	call nz,resetFX3
	jp fxreturn
_extfx
	and #3f				;make sure ptn # is valid
	push de				;preserve original fx ptn pointer
	
	ld hl,fxptntab
	add a,a
	add a,l
	ld l,a
IF MODEL != TI8X || MODEL != TI8XS	;fx ptn table crosses page boundary on 82/83
	jr nc,_skip
	inc h
_skip
ENDIF
	ld e,(hl)			;seq pointer fx to de
	inc hl
	ld d,(hl)
	
	ld a,#c9			;modify fxcont to contain a RET
	ld (fxcont),a
	
	ld a,(de)			;read fx #
	and #f				;mask out drum value
	inc de				;inc fx pointer
	call nz,fxextselect		;execute fx command
	inc de
	ld a,(de)
	and #f
	inc de
	call nz,fxextselect
	inc de
	ld a,(de)
	and #f
	inc de
	call nz,fxextselect
	inc de
	ld a,(de)
	and #f
	inc de
	call nz,fxextselect
	inc de
	ld a,(de)
	and #f
	inc de
	call nz,fxextselect
	
	ld a,#c3			;restore jump at fxcont
	ld (fxcont),a
	pop de				;restore fx ptn pointer
	jp fxreturn

	
fxextselect				;select fx
	dec a				;calculate jump
	add a,a				
	add a,a
	ld (_jump),a
_jump equ $+1
	jr $
	
fxExtJumpTab
	jp fx1
	nop
	jp fx2
	nop
	jp fx3
	nop
	jp fx4
	nop
	jp fx5
	nop
	jp fx6
	nop
	jp fx7			;fx7
	nop
	jp fx8			;fx8
	nop
	jp fx9			;fx9
	nop
	jp fxA
	nop
	ret			;fxB
	ds 3
	jp fxC
	nop
	jp fxD
	nop
	ret			;fxE
	ds 3
	jp fxF	
		
setright1
	ex af,af'
	ld a,lp_r
	ld (pan1),a
	ex af,af'
	rrca
	jp pch2

setleft1
	ex af,af'
	ld a,lp_l
	ld (pan1),a
	ex af,af'
	jp pch2
	
setright2
	ex af,af'
	ld a,lp_r
	ld (pan2),a
	ex af,af'
	rrca
	jp pch3

setleft2
	ex af,af'
	ld a,lp_l
	ld (pan2),a
	ex af,af'
	jp pch3

setright3
	ex af,af'
	ld a,lp_r
	ld (pan3),a
	ex af,af'
	rrca
	jp pchD

setleft3
	ex af,af'
	ld a,lp_l
	ld (pan3),a
	ex af,af'
	jp pchD
	
setrightD
	;ex af,af'
	ld a,lp_swl
	ld (panD),a
	ld a,lp_r
	ld (maskD),a
	;ex af,af'
	;rrca
	jp fxcont

setleftD
	;ex af,af'
	ld a,lp_swr
	ld (panD),a
	ld a,lp_l
	ld (maskD),a
	;ex af,af'
	jp fxcont

fxcont0
	ld (ptnreptpos),a	
fxcont
	jp fxreturn			;jp = #c3, ret = #c9

;*************************************************************************************
resetFX0				;reset Bxy effect
	xor a
	ld (ptnreptpos),a
	
resetFX					;reset all fx

	ld a,(musicData)		;reset speed
	ld l,a
	and #3f
	ld (cspeed),a
	ld a,l
	and #c0
	ld (cspeed-1),a

resetFX1
	ld a,#80			;reset phase shift
	ld (phaseshift1),a
	ld (phaseshift2),a
	ld (phaseshift3),a
	
resetFX2	
	ld a,lp_sw			;reset panning switches
	ld (panD),a
	
	ld a,lp_on
	ld (pan1),a
	ld (pan2),a
	ld (pan3),a
	
	ld a,lp_off
	ld (maskD),a

resetFX3
	ld a,#9f
	ld (syncSwitch),a
	
	ld a,#e6			;reset ch2 duty modulation setting 1
	ld (dutyModSwitch1),a
	
	ld a,#c6			;reset ch2 duty modulation setting 2	
	;ld (pwmswitch),a
	ld (dutyModSwitch2),a		
	
	ld a,#03			;reset drum counter mode
	ld (drumswap),a
	
	xor a
	ld (fxswap1),a			;reset A0x fx
	ld (fxswap2),a
	ld (pitchslide),a
	ld (pitchslide+1),a
	ld (drumswap2),a		;reset drum value mode
	;ld (fastpwmswitch),a		;reset fastpwm
	ld (dutyMod),a			;reset ch2 duty modulator
				
	ld hl,noXFX			;reset extended FX
	ld (xFX),hl

	ret				;must return with A=0 and Z-flag set
;*************************************************************************************
lpInvalidErr				;handle error caused by invalid loop point created by deleting row to loop to in livemode
	pop hl				;clean stack
	ld a,4
	jp errorHand
	
;*************************************************************************************
IF ((HIGH($+5))<(HIGH($+37)))
	org 256*(1+(HIGH($)))		;align to next page if necessary
.WARNING sample table crosses page boundary
ENDIF
	db 'XDRUM'

	ds 2				;needed for sample LUT offset calculation
	
samples				;sample table may not cross page boundary
	dw kickdr		;1 kick
	;dw text_mem
IF MODEL = TI82 || MODEL = TI8P
	dw 0			;2 snare/hat
	dw #569b		;3 metallic snare + low end
	dw #3c0			;4 short low noise/snare
	dw #6aa3		;5 "plick"	
	dw #5b24		;6 periodic noise
	dw #6ba4		;7 short clap/click
	dw #6940		;8 periodic noise/snare
	dw #67a0		;9 ???!
	dw #800			;a double shot
	dw #5e31		;b tiny kick
ENDIF
IF MODEL = TI83
	dw 8			;2 snare/hat
	dw #355		;dw #569b		;3 metallic snare + low end
	dw #4ca			;4 short low noise/snare
	dw #491		;dw #6aa3		;5 "plick"	
	dw #19a2	;dw #5b24		;6 periodic noise
	dw #20af	;dw #6ba4		;7 short clap/click
	dw #2361	;dw #6940		;8 periodic noise/snare
	dw #25a6	;dw #67a0		;9 ???!
	dw #2a40	;dw #800			;a double shot
	dw #2bae	;dw #5e31		;b tiny kick
ENDIF
IF MODEL = TI8X
	dw #6e			;2 snare/hat
	dw #12d1	;dw #569b		;3 metallic snare + low end
	dw #5fa		;dw #3c0			;4 short low noise/snare
	dw #13c0	;dw #6aa3		;5 "plick"	
	dw #154a	;dw #5b24		;6 periodic noise
	dw #16ad	;dw #6ba4		;7 short clap/click
	dw #2c60	;TODO dw #19b2	;dw #6940		;8 periodic noise/snare
	dw #17a9	;dw #67a0		;9 ???!
	dw #1d3f	;dw #800			;a double shot
	dw #1f73	;dw #5e31		;b tiny kick
ENDIF
	dw CsrTab		;c tiny laser
	dw CsrTab2		;d noise (better)
	dw fxptn00+(32*32)	;e user defined sample
usrdrum
	dw 0			;f user defined pointer

	;dw #300	;3 heavy snare			-- this one probably goes
	;dw initSCR	
	;dw #54d2	;d noise
	;dw #4e28	;e noise
	;dw #5fcd

;IF ((HIGH(samples-2))<(HIGH(usrdrum+2)))
;.ERROR sample table crosses page boundary
;ENDIF

kickdr
	db #80, #80, #70, #70, #60, #60, #50, #50, #40, #40, #40, #30, #30, #30, #30
	db #20, #20, #20, #20, #20, #10, #10, #10, #10, #10, #10, #8, #8, #8, #8, #8, #8, #8
	db #4, #4, #4, #4, #4, #4, #4, #4, #2, #2, #2, #2, #2, #2, #2, #2, #2, #0
	
musicData				;the music data
	include "music.asm"
