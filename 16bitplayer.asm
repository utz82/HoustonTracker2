
play
	call waitForKeyRelease		;keyhandler is very fast, so make sure there are currently no keys pressed
	
init					;sandboxing the actual player so we can handle keypresses vs looping quickly
	ld de,ptns
initrp					;init point for row play
	push de
	call resetFX

	ld hl,(musicData+1)		;load user (drum) sample pointer \ this will become redundant, (usrdrum) can be set on loading song
	ld (usrdrum),hl			;save it to drum table           / and if song is already in mem, it is then also already loaded

	ld a,(looprow)			;set loop point
	add a,a				;A*4
	add a,a
	ld l,a
	ld h,0
	ld de,ptns
	
	ld a,(de)			;while we're at it, check if player is set to start from an empty row (first pattern # = #ff)
	;inc a
	;ret z				;and exit if so
	
	add hl,de			;else, continue calculating loop point
	ld (looppoint),hl
	pop de
	
	inc a				;continue "empty row" check
	ret z

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

	
	;ld hl,ptntab		;10	;point to pattern position LUT
	ld h,HIGH(ptntab)
	add a,a			;4	;A=A*2
	;add a,l			;4
	ld l,a			;4
	;jr nc,psskip1		;12/7
	;inc h			;4

psskip1
	ld c,(hl)		;7	;seq pointer ch1 to bc
	inc hl			;6
	ld b,(hl)		;7
	push bc			;11

	
	inc de			;6	;point to ch3
	ld a,(de)		;7	;load ptn# ch3
	;ld hl,ptntab		;10
	add a,a			;4
	;add a,l			;4
	ld l,a			;4
	;jr nc,psskip2		;12/7
	;inc h			;4
	
psskip2
	ld c,(hl)		;7	;seq pointer ch3 to bc
	inc hl			;6
	ld b,(hl)		;7
	push bc			;11
	
	
	inc de			;6	;point to ch2
	ld a,(de)		;7	;load ptn# ch2
	;ld hl,ptntab		;10
	add a,a			;4
	;add a,l			;4
	ld l,a			;4
	;jr nc,psskip3		;12/7
	;inc h			;4

psskip3
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
	jr nc,psskip4		;12/7
	inc h			;4
psskip4
	ld c,(hl)		;7	;seq pointer fx to bc
	inc hl			;6
	ld b,(hl)		;7
	
	
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
reptpos equ $+1	
	ld a,0			;7	;decrement pattern row counter
	dec a			;4
	jr z,ptnselect0		;12/7	;if it has reached 0, move on to the next pattern -> NEED TO CORRECT STACK BEFORE JUMP!!!
	ld (reptpos),a		;13	;TODO: using DEC (HL) will be faster

rdnotesRP				;entry point for RowPlay
	ld hl,NoteTab		;10	;point to frequency LUT
	
	pop bc			;10	;ch1 ptn pointer to bc
	ld a,(bc)		;7
	inc bc			;6	;increment ptn pos pointer
	add a,a			;4
	add a,l			;4	;get offset in frequency LUT
	ld l,a			;4
	
	ld a,(hl)		;7	;lookup lo byte of frequency value
	add a,a			;4	;for channel 1, values are multiplied by 2
	ld (ch1),a		;13	;and store it to base counter
	inc l			;4
	;ld a,0			;7	;preserve carry
	ld a,(hl)
	adc a,(hl)		;7	;lookup and double add hi byte of frequency vaule
	;add a,(hl)		;7	;done: how about ld a,(hl), adc a,(hl)?
	ld (ch1+1),a		;13	;and store it to base counter
	
	ld l,0			;7	;reset freq.LUT pointer
	pop de			;10	;ch3 ptn pointer to de
	ld a,(de)		;7
	inc de			;6	;increment ptn pos pointer
	rla			;4	;can use rla here since carry is reset from previous add a,l op
	add a,l			;4
	ld l,a			;4
	
	ld a,(hl)		;7
	ld (ch3),a		;13
	inc l			;4
	ld a,(hl)		;7
	ld (ch3+1),a		;13
	
	exx			;4
	
	ld hl,NoteTab		;10	;reset freq.LUT pointer
	pop bc			;10	;ch2 ptn pointer to bc'
	ld a,(bc)		;7
	inc bc			;6
	rla			;4
	add a,l			;4
	ld l,a			;4
	
	ld a,(hl)		;7
	ld (ch2),a		;13
	inc l			;4
	ld a,(hl)		;7
	ld (ch2+1),a		;13
	
	pop de			;10	;fx ptn pointer to de
	ld a,(de)		;7	;read fx#
	
	and %11110000		;7	;check for drum trigger (lower nibble)
 	jp nz,drums		;17/10
	
	ld hl,drumres		;10	;reset drum by setting pointer to a 0 val TODO: seems messy, optimize
	ld (drumtrig),hl	;16	;	                                  TODO: ^^ 

drx	
	ld a,(de)		;7
	and %00001111		;7	;
	inc de			;6	;point to fx parameter
	jp nz,fxselect		;10
	
fxcont
	inc de			;6	;point to next row

	push de			;11	;stack 2			
	push bc			;11	;stack 3

	ld bc,0			;10	;BC' is add value for ch2, zero it

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
	
	ld (oldSP),sp		;20	;preserve SP  --> TODO: calculate this on entering player
ch1 equ $+1
	ld sp,#0000		;10	;load base frequency counter val
	ld (counterSP),sp	;20	;TODO: maybe remove, it's not that important...

drumres equ $+1	
	ld hl,0			;10	;HL holds "add" value for ch1, zero it; the 0-word also acts as stop byte for the drums
					;TODO: seems messy (see above)

mask1 equ $+2				;panning switches for drum and ch1
maskD equ $+3	
	ld ix,lp_msk		;14	;initialize output masks for those channels that use it (set to all channels off)

;*************************************************************************************
playnote				;synthesize and output current step
					
	ld a,(bc)		;7	;load sample byte
drumswap2
	nop			;4	;daa/cpl/etc
dru equ $+1
	add a,0			;7	;add current drum pitch counter
	ld (dru),a		;13	;and save it as current pitch counter
	
volswap1 equ $+1	
	ld a,ixh		;8	;output mask for drum channel - ld a,ixh = dd 7c, ld a,ixl = dd 7d
	out (link),a		;11	;output drum channel state
					;---- CH2: 104t	
muteD					;mute switch for drums
	jr nc,waitD		;12/7	;skip the following if result was <=#ff

drumswap				;switch for drum mode. inc bc = #03, dec bc = #0b, inc c = #0c, dec c = #0d, nop	
	inc bc			;6	;increment sample data pointer
panD equ $+1
	xor lp_sw		;7	;toggle output mask
	ld ixh,a		;8	;and update it
					;28t
outdr
;volswap2 equ $+1
	ld a,ixl		;8	;load output mask ch1 - ld a,ixl = dd 7d, ld a,ixh = dd 7c, or ixl = dd b5, and ixl = dd a5
	add hl,sp		;11	;add current counter to base freq.counter val. ch1 and save result in HL
mute1					;mute switch for ch1
	jr nc,out1		;12/7	;skip the following if result was <=#ffff
					;else						
pan1 equ $+1
	xor lp_sw		;7	;toggle output mask ch1
	ld ixl,a		;8	;and update it
out1
	out (link),a		;11	;output state ch1
					;----- DRUMS: 77/78/87/88t (old 85/95t) (old 105/115t)
	exx			;4	;back to the normal register set
	
	add iy,de		;15	;this is actually ch3
phaseshift3 equ $+1			;switch for phase shift/set duty cycle
	ld a,#80		;7	
	cp iyh			;8
mute3
	sbc a,a			;4	;result of this will be either #00 or #ff. for mute, swap #9f (sbc a,a) with #97 (sub a,a)
	or lp_off		;7	;TODO: useless on 8x, maybe can be eliminated on other models, too?
pan3 equ $+1
	and lp_on		;7	;and thus we derive the output state
pitchslide equ $+1
	ld hl,0			;10	;switch for pitch slide
	add hl,de		;11
	ex de,hl		;4
out3
	out (link),a		;11
					;---- CH1: 88t
	
ch3 equ $+1
	ld hl,#0000		;10	;and now, same as above but for ch2
	add hl,bc		;11
	ld b,h			;4
	ld c,l			;4	
phaseshift2 equ $+1
	ld a,#80		;7
pwmswitch				;ch2 PWM effect switch	
	add a,0			;7	;add a,n = #c6; adc a,n = #ce
	ld (phaseshift2),a	;13	
	cp b			;4
mute2
	sbc a,a			;4
	or lp_off		;7	;TODO: useless on 8x, maybe can be eliminated on other models, too?
pan2 equ $+1
	and lp_on		;7
out2
	out (link),a		;11
					;---- CH3: 89t
readkeys				;check if a key has been pressed
	in a,(kbd)		;11
	cpl			;4	;COULD IN THEORY OPTIMIZE THIS AWAY
	or a			;4
	jr nz,keyPressed	;12/7	;and exit if necessary

reentry	
	exx			;4

; 	dec de
; 	ld a,d
; 	or e
; 	jp nz,playnote
	dec e			;4	;update speed counter - slightly inefficient, but faster on average than dec de\ld a,d\or e, and gives better sound
	jr nz,playnote		;12/7   ;TODO: worth using jp??? worth retracting to dec de..? (+8t)?
				;
	dec d			;4
	jp nz,playnote		;10
				;
;*************************************************************************************
keyPressed
oldSP equ $+1
	ld sp,0				;retrieve SP
rowplaySwap equ $+1			;switch for jumping to exitRowplay instead, jp z = #ca, ret = #c9
	jp z,rdnotes			;z-flag will be set when speed counter has reached 0
	
	push de				;preserve all counters
	push bc
	exx
	push de
	push hl
	push bc

	call keyhand
	
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
fxswap1
fxswap2 equ $+1
	ld a,4			;7	;swap with rlc h (cb 04) for noise/glitch effect, with rlc l (cb 05) for phase effect - ld a,n = c6
	jp outdr		;10
				;17+12=29

;*************************************************************************************
drums					;select drum
	ld hl,samples-2			;point to beginning of sample pointer table - 2 (because minimum offset will be +2)
	rra				;divide drum # by 8 to get offset in table (carry is cleared before calling drum select)
	rra
	rra
	add a,l				;add offset to (h)l
	ld l,a
	ld a,(hl)			;load drum data pointer into bc
	ld (drumtrig),a
	inc l
	ld a,(hl)
	ld (drumtrig+1),a
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
	jp fxcont			;fx7
	nop
	jp fxcont			;fx8
	nop
	jp fxcont			;fx9
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
	ld (cspeed),a
	jp fxcont

fxB					;#0b = break cmd
	ld a,(reptpos)
	cp #10
	jp nz,ptnselect		;10	;select next ptn if found  ATTN: leaves register set swapped!
	jp fxcont
	
fx1					;#01 = set panning
	ld a,(de)

pch1
	rrca
	jp c,setright1
	rrca
	jp c,setleft1
	ex af,af'
	ld a,lp_sw
	ld (pan1),a
	ld a,lp_off
	ld (mask1),a
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
	ex af,af'
	ld a,lp_sw
	ld (panD),a
	ld a,lp_off
	ld (maskD),a
	ex af,af'
	jp fxcont
	
fx2					;pitch slide up
	ld a,(de)
	ld (pitchslide),a
	xor a
	ld (pitchslide+1),a
	jp fxcont
	
fx3					;pitch slide up
	ld a,(de)
	neg
	ld (pitchslide),a
	ld a,#ff
	ld (pitchslide+1),a
	jp fxcont	

fx4					;ch2 pwm mode
	ld a,(de)
	or a
	jr z,_resetpwm
	ld a,#ce
	ld (pwmswitch),a
	jp fxcont
_resetpwm
	ld a,#c6
	ld (pwmswitch),a
	ld a,#80
	ld (phaseshift2),a
	jp fxcont

fx5					;duty cycle ch2
	ld a,(de)
	ld (phaseshift2),a	
	jp fxcont
	
fx6					;duty cycle ch3
	ld a,(de)
	ld (phaseshift3),a
	jp fxcont

fxA					;ch1 "glitch" effect
	ld a,(de)
	or a
	jr z,_Askip
	dec a
	ld a,5
	jr nz,_Afast

	dec a
_Afast
	ld (fxswap2),a
	ld a,#cb
	ld (fxswap1),a
	jp fxcont
_Askip
	ld a,#c6
	ld (fxswap1),a
	jp fxcont
	
	
fxC					;set drum mode.
	ld a,(de)
	and #0f				;calculate jump
	add a,a
	add a,a
	ld (_jumpx),a
_jumpx equ $+1
	jr $

_fxCx0
	ld a,0				;nop (default)
	jr _fxCxx
_fxCx1
	ld a,#27			;daa
	jr _fxCxx
_fxCx2
	ld a,#8f			;add a,a
	jr _fxCxx
_fxCx3
	ld a,#1f			;rra
	jr _fxCxx
_fxCx4
	ld a,#2f			;cpl
	jr _fxCxx
_fxCx5
	ld a,#79			;ld a,c
	jr _fxCxx
_fxCx6
	ld a,#89			;add a,c
	jr _fxCxx
_fxCx7
	ld a,#88			;add a,b
	jr _fxCxx
_fxCx8
	ld a,#90			;sub b
	jr _fxCxx
_fxCx9
	ld a,#91			;sub c
	jr _fxCxx
_fxCxa
	ld a,#a0			;and b
	jr _fxCxx
_fxCxb
	ld a,#a1			;and c
	jr _fxCxx
_fxCxc
	ld a,#b0			;or b
	jr _fxCxx
_fxCxd
	ld a,#b1			;or c
	jr _fxCxx
_fxCxe
	ld a,#a8			;xor b
	jr _fxCxx
_fxCxf
	ld a,#a9			;xor c

_fxCxx
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

_fxC0x
	ld a,#03			;inc bc
	jr _fxCyy	
_fxC1x
	ld a,#0b			;dec bc
	jr _fxCyy	
_fxC2x
	ld a,#0c			;inc c
	jr _fxCyy	
_fxC3x
	ld a,#0d			;dec c
	jr _fxCyy
_fxC4x
	xor a				;nop

_fxCyy
	ld (drumswap),a					
	jp fxcont

fxD					;ch1/d volume shift
; 	ld a,(de)
; 	or a
; 	jr z,_Vreset
;
; 	dec a				;D01 = double vol ch1
; 	jr nz,_fxD2
; 	ld a,#7d
; _fxDxx
; 	ld (volswap2),a
; 	ld (volswap1),a
; 	jp fxcont
;
; _fxD2					;D02 = double vol drums
; 	dec a
; 	jr nz,_fxD3
; 	ld a,#7c
; 	jr _fxDxx
; 	
; _fxD3					;D03 = drums OR ch1
; 	dec a
; 	ld a,#7c
; 	ld (volswap1),a
; 	jr nz,_fxD4	
; 	ld a,#b5
; 	db #c2				;jp nz,...
; 	
;
; _fxD4					;D04 = drums AND ch1
; 	ld a,#a5
; 	ld (volswap2),a
; 	jp fxcont	
; 	
; _Vreset
; 	ld a,#7c
; 	ld (volswap1),a
; 	inc a
; 	ld (volswap2),a
; 	jp fxcont
	
	jr nz,_Vskip
	ld a,#7c
	db #c2
_Vskip
	ld a,#7d
	ld (volswap1),a
	jp fxcont

fxE					;reset all fx
	ld a,(de)
	or a
	call z,resetFX			;resetFX always returns with A=0 and Z-flag set	
	dec a
	call z,resetFX1
	dec a
	call z,resetFX2
	call nz,resetFX3
	jp fxcont
		
setright1
	ex af,af'
	ld a,lp_swr
	ld (pan1),a
	ld a,lp_r
	ld (mask1),a
	ex af,af'
	rrca
	jp pch2

setleft1
	ex af,af'
	ld a,lp_swl
	ld (pan1),a
	ld a,lp_l
	ld (mask1),a
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
	ex af,af'
	ld a,lp_swr
	ld (panD),a
	ld a,lp_r
	ld (maskD),a
	ex af,af'
	rrca
	jp fxcont

setleftD
	ex af,af'
	ld a,lp_swl
	ld (panD),a
	ld a,lp_l
	ld (maskD),a
	ex af,af'
	jp fxcont

;*************************************************************************************	
resetFX					;reset all fx

	ld a,(musicData)		;reset speed
	ld (cspeed),a

resetFX1
	ld a,#80			;reset phase shift
	ld (phaseshift2),a
	ld (phaseshift3),a
	
resetFX2	
	ld a,lp_sw			;reset panning switches
	ld (pan1),a
	ld (panD),a
	
	ld a,lp_on
	ld (pan2),a
	ld (pan3),a
	
	ld a,lp_off			;TEST
	ld (mask1),a
	ld (maskD),a

resetFX3	
	ld a,#c6			;reset ch1 fx
	ld (fxswap1),a
	;ld (fxswap2),a
	
	ld (pwmswitch),a		;reset pwm sweep
	
	ld a,#7c			;reset volume shift drums/ch1
	ld (volswap1),a
	;inc a
	;ld (volswap2),a
	
	ld a,#03			;reset drum counter mode
	ld (drumswap),a
	
	xor a
	ld (pitchslide),a
	ld (pitchslide+1),a
	ld (drumswap2),a		;reset drum value mode

	ret				;must return with Z-flag set
;*************************************************************************************
lpInvalidErr				;handle error caused by invalid loop point created by deleting row to loop to in livemode
	pop hl				;clean stack
	ld a,4
	jp errorHand
	
;*************************************************************************************
IF ((HIGH($))<(HIGH($+32)))
	org 256*(1+(HIGH($)))		;align to next page if necessary
.WARNING sample table crosses page boundary
ENDIF
	;ds #20

	ds 2				;needed for sample LUT offset calculation
	
samples				;sample table may not cross page boundary
	dw kickdr		;1 kick
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
