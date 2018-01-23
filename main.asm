;************************************************************************************
;HOUSTONTRACKER 2.24.00
;by utz * irrlichtproject.de/houston
;************************************************************************************

;************************************************************************************
;OS/shell settings
;************************************************************************************

TI82 EQU 1
TI83 EQU 2
TI8X EQU 3
TI8P EQU 4
TI8XS EQU 5

IF MODEL = TI82		
include "include/ti82.inc"
ENDIF

IF MODEL = TI8P
include "include/ti82parcus.inc"
ENDIF

IF MODEL = TI83
include "include/ti83.inc"
ENDIF

IF MODEL = TI8X
include "include/ti8xp.inc"
ENDIF

IF MODEL = TI8XS
include "include/ti8xs.inc"
ENDIF

	db "HT 2.24", 0

;************************************************************************************
;APD_BUF scratch pad and other equates


CharString	equ apd_buf+#00		;2	2 character codes for printing
FirstLineMS	equ apd_buf+#02		;1	first line of sng sequence to be displayed on main screen
CPS		equ apd_buf+#03		;1	start of copy block
CPE		equ apd_buf+#04		;1	end of copy block

SourcePtn	equ apd_buf+#05		;1	number of the source pattern (for copying to another pattern)


CScrType	equ apd_buf+#06		;1	current type of screen (0=main, 1=ptn, 2=fxptn)
AlphaFlag	equ apd_buf+#07		;1	#a0 if Alpha has been pressed, else 0

PlayerFlag	equ apd_buf+#08		;1	0 if player is stopped, 1 if running

COct		equ apd_buf+#09		;1	current octave
CPtn		equ apd_buf+#0a		;1	current pattern

CSRow		equ apd_buf+#0b		;1	current row in ptn sequence
CSCol		equ apd_buf+#0c		;1	current column in ptn sequence

CPRow		equ apd_buf+#0d		;1	current row in pattern
CPCol		equ apd_buf+#0e		;1	current column in pattern

CsrPos		equ apd_buf+#0f		;1	cursor position
OldCsrPos	equ apd_buf+#10		;1	temp backup cursor position on main screen
OldCsrPos2	equ apd_buf+#11		;1	temp backup current cursor pos (for handling vars)

CurVal		equ apd_buf+#12		;1	current cursor bitmap value (#0f, #f0, or #ff)

MuteState	equ apd_buf+#13		;1	mute state of channels (bit 7 = drums, bit 6 = ch3, bit 1 = ch2, bit 0 = ch1)

InputType	equ apd_buf+#14		;1	input type (0=regular, 1=single digit, 2=double digit, 4=word)

StateSelect	equ apd_buf+#15		;1	number of the currently selected save slot

AutoInc		equ apd_buf+#16		;1	auto inc mode (1 = off, 0 = on)

RowPlay		equ apd_buf+#17		;1	RowPlay mode (0 = off, #ff = on)

LastKey		equ apd_buf+#18		;1	#a0 if last key set ALPHA mode on, else 0

reptpos		equ apd_buf+#19		;1	number of remaining rows in pattern during playback

SynthMode	equ apd_buf+#20		;1	Synth mode (0 = off, #ff = on).

;ReInit		equ apd_buf+#1a		;1	flag for reinitialization  

PrintBuf	equ ops			;6	print bitmap buffer

;FontLUT	equ 256*((HIGH(apd_buf))+1)	;font LUT

NoteTab		equ 256*((HIGH(apd_buf))+2)	;note LUT

CsrTab		equ 256*((HIGH(graph_mem))+1)	;cursor position LUT

IF MODEL = TI83					;fx pattern screen cursor position LUT
CsrTab2		equ graph_mem			;generated on the lowest page of graph_mem on TI83
ENDIF
IF MODEL = TI82	|| MODEL = TI8P			;generated on the lowest page of apd_buf on TI82
CsrTab2		equ #80+(256*(HIGH(apd_buf)))
ENDIF
IF MODEL = TI8X || MODEL = TI8XS
CsrTab2		equ 256*((HIGH(text_mem2))+1)	;generated on the second page of statram on TI83P
ENDIF

ptntab		equ 256*((HIGH(graph_mem))+2)	;pattern table

fxptntab	equ text_mem2

;************************************************************************************
;actual code starts here

begin						;initialize stuff
	include "init.asm"
	call waitForKeyRelease			;make sure no key is currently pressed before entering keyhandler

mainlp						;the main loop. yes, it's very simple ;)
	call keyhand
	
	jp mainlp

;************************************************************************************

	include "keyhand.asm"			;keyhandler
	
;************************************************************************************
exit						;exit HT2
	ld a,#d2				;reset mute switches (#30 = jr nc,..)
	;ld (mute1),a
	ld (muteD),a
	ld a,#9f				;#9f = sbc a,a
	ld (mute1),a
	ld (mute2),a
	ld (mute3),a
	
	ld a,lp_on				;switch link port lines high again
	out (link),a


exitSP equ $+1					;reset stack
	ld sp,0
	pop ix					;restore index registers
	pop iy

	ei					;done automatically by CrASH
	ret					;and byebye


;************************************************************************************
;SUBROUTINES
;************************************************************************************
rowPlay						;rowPlay subroutine
	call waitForKeyRelease

	ld a,(PlayerFlag)			;check if player is running
	or a	
	jp nz,_ignore				;and ignore rowPlay if that's the case	
	
	cpl					
	ld (PlayerFlag),a			;set PlayerFlag to prevent double call
	
	;call waitForKeyRelease
	
	xor a
	ld h,a
	ld d,a
	ld a,(OldCsrPos)
	call findCurrLineNoSeq			;find current line in sequence
	
	ld de,3					;reading in backwards, therefore add 3 to offset
	add hl,de
	ex de,hl
	

	ld bc,fxptn00
	
	ld a,(CsrPos)
	sub #50
	cp #f
	jr c,_skipadj	
	sub 8
_skipadj
	add a,a
	ld l,a
	ld h,0
	add hl,bc
	ld b,h
	ld c,l
	
	ld a,(de)
	add a,a					;a*2
	call calcPtnOffset+2
	push hl					;fx - ;stack+0 = ch1, stack+2 = ch2, stack+4 = ch3, stack+6 = fx
	
	ld bc,ptn00
	
	ld a,(CsrPos)
	sub #50
	cp #f
	jr c,_skipdiv
	sub 8
_skipdiv	
	ld l,a
	ld h,0
	add hl,bc
	ld b,h
	ld c,l
	
	call calcPtnOffset
	push hl					;ch3
	
	call calcPtnOffset
	push hl					;ch2
	
	call calcPtnOffset
	push hl					;ch1
	
	ld (oldSP),sp				
	
	ld hl,_next
	ld (rowplaySwap),hl

	xor a
	out (kbd),a
	
	jp rdnotesRP				;call player
_next	
	pop hl					;clean stack and reset switch
	pop hl
	pop hl
	pop hl
	ld hl,rdnotes
	ld (rowplaySwap),hl
	xor a					;clear PlayerFlag
	ld (PlayerFlag),a
_ignore	
	;jp waitForKeyRelease
	ret

calcPtnOffset					;calculate offsets in note data
	dec de
	ld a,(de)
	add a,a
	ld l,a
	ld h,0
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,bc
	ret
;************************************************************************************
confirmAction					;wait for confirmation/abortion of user action
						;IN: nothing | OUT: carry reset if confirmed, else abort

	ld de,#29b8				;print CONF message
	call setXY
	ld de,#0c00				;CO
	call printDE
	ld de,#2ab8
	call setXY
	ld de,#110f				;NF
	call printDE

_rdkeys
	ld a,#ef				;read key 0
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	jp nc,_cancel				;if pressed, cancel user action

	ld a,#f7				;read key .
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	jp nc,_confirm				;if pressed, confirm user action
	jr _rdkeys				;if no key pressed, try again
	
_cancel
	scf					;set carry
	
_confirm					;if user action confirmed, carry is already reset
_exitc
clrMsgArea					;clear message area
	push af					;preserve flags
	ld de,#29b2				;delete CONF message and rest of msg area
	call setXY
	call clearPrintBuf
	call printBuf
	call printBuf
	ld de,#2ab8
	call setXY
	call printBuf
	ld de,#2bb2
	call setXY
	call printBuf

	pop af
	ret

;************************************************************************************
calculateCopyParams				;calculate block start, length and target line #
						;IN: nothing | OUT: block start in DE, block length in BC and HL, current line # in A
	exx
	ld a,d					;D' holds old block start
	exx

	;ld a,(CPS)				;calculate block start in memory
	ld l,a
	ld h,0
	add hl,hl
	add hl,hl
	ex de,hl				;block start - base offset now in DE
	
	;ld a,(CPE)				;calculate block length in memory
	exx
	ld a,e					;E' holds old block end
	exx
	
	ld l,a
	ld h,0
	add hl,hl
	add hl,hl
	;xor a					;TODO: optimzed 15-08-19 - carry should never be set at this point, so we don't need to reset it
	sbc hl,de
	inc hl
	inc hl
	inc hl
	inc hl					;block length now in HL
	
	call getCurrLineNo			;current line # now in A
	
	ld b,h					;block length now in BC
	ld c,l
	ret


;************************************************************************************
getCurrLineNo					;calculate the current line number
						;IN: nothing | OUT: current line number in A | DESTROYED: C
	ld a,(CsrPos)
	and %11111000
	rra
	rra
	rra
	ld c,a
	ld a,(FirstLineMS)
	add a,c
	ret
	

;************************************************************************************
findCurrLine					;find the current line in the sequence
						;IN: nothing | OUT: pointer to start of line in HL
	xor a
	ld h,a
	ld d,a
						
	ld a,(CsrPos)
	ld (OldCsrPos),a
	
findCurrLineNoSeq				;entry point for finding the current row when not on seq.scr
	and %11111000				;clear lower 3 bits of cursor pos value to get to the start of the line and clear carry
	rra					;divide by two
	ld e,a					;store in DE
	ld a,(FirstLineMS)			;read current first line
	ld l,a					;store in HL

	add hl,hl				;HL*4 to get offset of first line
	add hl,hl
	add hl,de				;add DE to get offset of current line
	ld de,ptns				;add base pointer
	add hl,de				;sequence pointer now in HL
	ret

;************************************************************************************
findNextUnused					;find next unused ptn in sequence
						;IN: first # to check in A | OUT: next unused in A, Z if no unsed patterns found
	ld hl,ptns
	ld bc,256*4
_chklp
	cpi					;iterate through the pattern sequence
	jr z,_used				;exit loop if match found (pattern used)
	cpi
	jr z,_used
	cpi
	jr z,_used
	cpi					;every 4th pattern is an fx pattern, so it's ignored
	ret po					;return if pattern was unused
	jr _chklp

_used	
	inc a					;if pattern was used
	cp #80					;check if all patterns have been checked
	ret z					;and return if that's the case
	jr findNextUnused			;else, check next pattern
		

isPtnFree					;check if a pattern is free
						;IN: ptn # in A | OUT: Z if free, NZ if not free
						
	add a,a					;calculate offset
; 	ld l,a					;pattern # *2 to HL
; 	ld h,0			
; 	add hl,hl
; 	add hl,hl
; 	add hl,hl
; 	ld de,ptn00				;add base pointer
; 	add hl,de

	ld h,HIGH(ptntab)			;look up pattern address			
	ld l,a
	ld a,(hl)
	inc hl
	ld h,(hl)
	ld l,a
	
	ld b,#10
	xor a
	
chklp
	cp (hl)					;check value
	jr nz,_notfree				;exit loop if != 0
	inc hl					;else, increment pointer and check next val
	djnz chklp
_notfree		
	ret					;return with Z set if all values have been checked
	

findNextUnusedFX					;find the next free fx pattern
						;IN: first # to check in A | OUT: next unused in A, Z if no unsed patterns found
	ld hl,ptns
	ld bc,256*4
_chklp
	cpi
	cpi
	cpi
	cpi
	jr z,_used
	ret po
	jr _chklp

_used	
	inc a					;if pattern was used
	cp #40					;check if all patterns have been checked
	ret z					;and return if that's the case
	jr findNextUnusedFX			;else, check next pattern

isFxPtnFree					;check if a pattern is free
						;IN: ptn # in A | OUT: Z if free, NZ if not free
						
; 	add a,a			;4		;calculate offset		;TODO: can use pattern lookup table instead of this!
; 	add a,a			;4
; 	ld l,a			;4		;pattern # *4 to HL
; 	ld h,0			;7
; 	add hl,hl		;11
; 	add hl,hl		;11
; 	add hl,hl		;11
; 	ld de,fxptn00		;10		;add base pointer
; 	add hl,de		;11
; 				;73
	
	
	ld de,fxptntab		;10
	add a,a			;4
	ld h,0			;7
	ld l,a			;4
	add hl,de		;11
	ld a,(hl)		;7
	inc hl			;6
	ld h,(hl)		;7
	ld l,a			;4
				;60t
	
	ld b,#20
	jr chklp-1

;************************************************************************************
errorHand0				;clear StateSelect (when entering from LOAD/SAVE)
	ex af,af'
	ld a,#16
	ld (StateSelect),a
	ex af,af'

errorHand				;error handler
					;IN: A - error code
	ld d,#0e
	ld e,a

printMsg				;print a message to the message area
					;IN: DE - message	
	push de				
					
	ex af,af'			;TODO: preserving A for what?
	ld de,#29b2
	call setXY
	ex af,af'
	
	pop de

	jp printDE


;************************************************************************************
printMute12					;print Mute state ch1/2
	ld de,#2a88
	call setXY
	
	ld de,#0102
	ld a,(MuteState)
	rra
	jr nc,_skip
	ld d,#16
_skip
	rra
	jr nc,_skip2
	ld e,#16
_skip2
	jp printDE
	

printMute3D					;print Mute state ch3/4
	ld de,#2b88
	call setXY
	
	ld de,#030D
	ld a,(MuteState)
	rla
	jr nc,_skip
	ld e,#16
_skip
	rla
	jr nc,_skip2
	ld d,#16
_skip2
	jp printDE

;************************************************************************************
initSeqCsr
	ld a,(OldCsrPos)		;reset cursor pos
	ld (CsrPos),a

printCsr				;print the cursor
	ld hl,CsrTab			;load pointer to CsrPos LUT

csrTypeSwap equ $+1
	ld c,#0f			;cursor bitmask
	ld a,(CsrPos)
	add a,a				;A=A*2
	add a,l
	ld l,a
	ld a,(hl)
	or a
	jp p,_skip			;if bit 7 is set
	ld c,#f0			;change bitmask to #f0
	and %01111111			;clear bit 7
_skip
	ld d,a	
	inc l
	ld e,(hl)	
	call setXY

IF MODEL = TI82	
	ld a,(hl)			;waste some time
	ld a,(hl)
ELSE
	call lcdWait3
ENDIF
	ld a,c
	out (lcd_data),a
	ret

printCsr2				;alternative table pointer for FX screen
	ld hl,CsrTab2
	jr printCsr+3
	
delCsr2					;alternative table pointer for FX screen
	ld hl,CsrTab2
	jr delCsr+3

delCsr					;delete the cursor
	ld hl,CsrTab			;load pointer to CsrPos LUT
	ld a,(CsrPos)
	add a,a				;A=A*2
	add a,l
	ld l,a
	ld a,(hl)
	and %01111111			;clear bit 7
	ld d,a	
	inc l
	ld e,(hl)	
	call setXY

IF MODEL = TI82	
	ld a,(hl)
	ld a,(hl)
ELSE
	call lcdWait3
ENDIF
	xor a
	out (lcd_data),a	
	ret

;************************************************************************************
divNoteVal				;split note value into octave and val w/in the octave
					;IN: note val in A | OUT: octave in B, note val in C
	
	ld b,#ff
	dec a				;0=silence, so lowest actual note val is 1 -> decrement by 1
_divlp					;effectively dividing A by 12
	inc b				;increment octave
	ld c,a				;preserve remainder
	sub 12				;subtract 12
	jr nc,_divlp			;loop until result was <0

	ret

;************************************************************************************
printFxScr				;print an FX pattern screen

	call clrMsgArea			;clear message area
	
	ld a,LOW(kjumptab)+16		;set dirkey jump table pointer offset
	ld (kdirswitch),a
	
	ld a,(CsrPos)			;preserve cursor pos on seq.scr
	ld (OldCsrPos),a

	ld a,2				;set current screen type
	ld (CScrType),a

	call printSingleLN

_header					;printing ptn#/curr. octave info
	ld de,#2182
	call setXY
	
	ld de,#1214			;PT(N)
	call printDE
	
	ld de,#2382
	call setXY
	
	call getSeqOffset		;find pattern#
	
	ld a,(CPtn)			;load current ptn#
	cp #40				;if it's invalid (> #3f)
	jp nc,printSeqScr		;default to sequence screen

printFxScrNoInit			;init point when cycling through patterns
					
	call printChars			;print ptn#
	
	ld de,#218e
	call setXY

	ld a,(CPtn)
	ld hl,fxptntab
	call findCurrFxPtn		;get the pattern pointer
	
	push de				;preserve pointer for printing fx params later

	ex de,hl

	call printFXP
	
	ld de,#258e
	call setXY
	
	call printFXP
	
	ld de,#228e
	call setXY
	
	pop hl
	inc hl
	
	call printFXP
	
	ld de,#268e
	call setXY
	
	call printFXP
	
	xor a				;reset cursor pos
	ld (CsrPos),a
	call printCsr2			;initialize cursor 
	
	call waitForKeyRelease
	jp keyhand

printFXP				;print FX params
	ld b,8
_lp
	ld a,(hl)
	inc hl
	inc hl
	push hl
	push bc
	call printCharsNoFF
	pop bc
	pop hl
	djnz _lp
	
	ret


;************************************************************************************
findCurrPtn				;locate the currently selected pattern in memory
					;IN: nothing | OUT: pattern pointer in DE

	ld a,(CPtn)			;read current ptn# TODO: optimize by moving this to after the findCurrFxPtn label

findPtn					;find ptn in memory
	ld hl,ptntab		;10	;point to pattern position LUT

findCurrFxPtn				;entry point for finding fx pattern. A and HL need to be preset

	add a,a			;4	;A=A*2
	add a,l			;4
	ld l,a			;4
	jr nc,_skip1		;12/7
	inc h			;4
_skip1					;ptn pointer now at (HL)

	ld e,(hl)
	inc hl
	ld d,(hl)			;ptn pointer now in DE
	ret

;************************************************************************************
getSeqOffset				;get offset in ptn sequence based on current cursor pos on seq.screen
					;IN: nothing | OUT: sequence pointer in HL, [current] ptn# in A,(CPtn) | destroyed: DE

	xor a				;clear carry
	ld h,a				;ld h,0
	ld d,a	
	ld a,(CsrPos)			;read old cursor position (from seq.scr)
	rra				;divide by 2
	ld e,a
	ld a,(FirstLineMS)		;check first line on seq scr
	ld l,a
	add hl,hl			;offset*4
	add hl,hl
	add hl,de			;+ position offset = total offset in ptn sequence
	ld de,ptns			;add ptn sequence base
	add hl,de			;seq.pointer now in HL
	
	ld a,(hl)			;load ptn# into A
	ld (CPtn),a			;store it in (CPtn)
	ret

;************************************************************************************


printPtnScr				;print a pattern screen

	call clrMsgArea			;clear message area

	ld a,LOW(kjumptab)+8		;set dirkey jump table pointer offset
	ld (kdirswitch),a

	ld a,1				;set current screen type
	ld (CScrType),a
	
	ld a,(CsrPos)
	ld (OldCsrPos),a
	
	call printSingleLN
	
_header					;printing ptn#/curr. octave info
	ld de,#2182
	call setXY
	
	ld de,#1214			;PT(N)
	call printDE
	
	ld de,#000c			;OC(T)
	call printDE
	
	ld de,#2382
	call setXY
	
	call getSeqOffset
	
	ld a,(CPtn)			;current ptn#
	cp #80				;if pattern# > #7f
	jp nc,printSeqScr		;default to sequence screen
	
printPtnScrNoInit			;init point when cycling through patterns
	
	call printChars
	
	ld a,(COct)			;current octave
	call printCharL

printPtnScrBasic			;init point when not reprinting ptn nr, octave etc.
	ld de,#218e
	call setXY
	

	call findCurrPtn
	
	push de				;preserve pointer for printing octave #s later

	call printNoteNames
	ex de,hl
	ld de,#258e
	call setXY
	
	ex de,hl
	call printNoteNames

	ld de,#228e
	call setXY

	pop de				;retrieve pattern pointer
		
	call printOctaves
	
	push de
	ld de,#268e
	call setXY
	
	pop de
	call printOctaves
	
	ld a,#50			;init cursor pos
	ld (CsrPos),a
	
	ld a,#ff			;init cursor type
	ld (csrTypeSwap),a	
	
	call printCsr
	
	call waitForKeyRelease
	jp keyhand


printOctaves
	ld b,8
_lp
	ld a,(de)			;read note byte
	inc de				;point to next byte
	push de				;preserve pattern pointer
	push bc				;preserve counter
	or a
	jr nz,_skip2			;if note val = 0
	ld a,#16			;print a dash
	jr _skip3
_skip2
	call divNoteVal			;octave val returned in B
	
	ld a,b				;get it into A
_skip3
	ex af,af'
	call clearPrintBuf
	ex af,af'
	call printCharLNC		;and print it

	pop bc
	pop de
	djnz _lp
	ret




printNoteNames
	ld b,8
_lp
	ld a,(de)			;read note byte
	inc de
	push de
	push bc
	or a
	jr nz,_skip2			;if note val = 0
	ld d,#16			;load dashes into DE
	ld e,d
	jr _skip3
_skip2
	call divNoteVal			;note val returned in C
	
	ld a,c				;find note name
	ld hl,notenames			;point to note name LUT
	add a,a				;A=A*2
	ld e,a
	ld d,0
	add hl,de			;add offset to LUT pointer
	
	ld d,(hl)			;get string into DE
	inc hl
	ld e,(hl)

_skip3
	call printDE			;and print it
	
	pop bc
	pop de
	djnz _lp
	ret




printSingleLN				;print single digit line numbers

	ld de,#208e
	call setXY

	xor a				;starting with line 0
	ld b,8
	
	call printSingleLP		;print the first column
	
	ld de,#248e
	call setXY
	
	ld a,8				;starting with 8, print 2nd column
	ld b,a
	call printSingleLP
		
	ret

printSingleLP				;print loop for printing single digit line #s
	push af
	push bc
	call printCharL
	pop bc
	pop af
	inc a
	djnz printSingleLP
	ret

;************************************************************************************
printSeqScr				;print the sequence (main) screen
	;call clrMsgArea			;clear message area

	ld a,LOW(kjumptab)		;reset dirkey jump table pointer offset
	ld (kdirswitch),a

	xor a
	ld (CScrType),a			;set current screen type
	ld (InputType),a		;reset input type
	
	ld a,#0f			;init cursor type
	ld (csrTypeSwap),a

printSeqScr0
printLineNumbers			;print line numbers on main screen	
	ld de,#2082
	call setXY
	
	ld b,10				;10 lines to print	
	ld a,(FirstLineMS)		;load first line number
	
_plnlp
	push af
	push bc
	call printCharsNoFF
	pop bc
	pop af
	inc a
	djnz _plnlp
	
printPtnSequence
	ld de,#2182			;ch1
	call setXY
	
	ld hl,ptns			;point HL to start of ptn sequence
	ld a,(FirstLineMS)		;get first row to be printed
	ld e,a				;load into DE
	ld d,0
	add hl,de			;first row * 4 = offset in ptn sequence
	add hl,de
	add hl,de
	add hl,de
	
	ld c,10				;printing 10 rows for each column
	call printSeqColR		;print upper nibbles
	ld de,#2282
	call setXY
	call printSeqColL		;print lower nibbles
	
	ld de,#2382			;ch2
	call setXY

	inc hl				;increment to point to next channel
	push hl				;preserve seq. pointer (printSeqCol doesn't do this for speed reasons)
	call printSeqCol
	
	ld de,#2482			;ch3
	call setXY
	pop hl
	
	inc hl
	call printSeqColR
	ld de,#2582			;ch2
	call setXY
	call printSeqColL
	
	ld de,#2682			;fx-ch
	call setXY

	inc hl
	call printSeqCol
	
	ld a,(OldCsrPos)
	ld (CsrPos),a

	ret

;************************************************************************************
printSeqColR				;print one right half-column of pattern sequence
	push hl				;preserve seq. pointer
	ld b,c				;printing 10 rows for each column
_ppslp
	push hl				;preserve sequence pointer
	push bc				;preserve counter
	ld a,(hl)			;load byte to print
	call printCharR			;print upper nibble, right-aligned
	pop bc				;retrieve counter
	pop hl				;retrieve seq. pointer
	ld de,4				;increment seq. pointer (by 4, because each row of the sequence is 4 bytes long)
	add hl,de
	djnz _ppslp			;loop until 10 rows have been printed
	pop hl				;retrieve seq. pointer
	ret


printSeqColL				;print one left half-column of pattern sequence	
	push hl
	ld b,c
_ppslp
	push hl
	push bc
	ld a,(hl)
	call printCharL
	pop bc
	pop hl
	ld de,4
	add hl,de
	djnz _ppslp
	pop hl
	ret


printSeqCol				;print one full column of pattern sequence	
	ld b,c
_ppslp
	push hl
	push bc
	ld a,(hl)
	call printChars
	pop bc
	pop hl
	ld de,4
	add hl,de
	djnz _ppslp
	ret

;************************************************************************************
printCharR
	ex af,af'			;preserve byte to be printed
	call clearPrintBuf		;clear print buffer
	ex af,af'			;retrieve byte to be printed
	call hex2charU			;convert upper nibble
	ld c,1				;init registers for setting up the print buffer (c=number of digits to print, b'=#83 signals shifted char)
	exx
	ld b,#8a
	call setupPB1char		;setup print buffer
	jr printBuf			;and finally print what's in the buffer

	
printCharL
	ex af,af'
	call clearPrintBuf
	ex af,af'
	call hex2charL
printCharLNC
	ld c,1
	exx
	ld b,0
	call setupPB1char
	jr printBuf



printCharsNoFF
	call hex2charNoFF
	jr printCharsNoConvert

printChars				;print the 2 characters in (CharString)
	call hex2char			;convert to chars
	
printCharsNoConvert			;print (CharString) without converting ASCII first
	ld de,(CharString)		;load chars into DE
	
printDE					;printing with custom value in DE
	call setupPrintBuf		;convert to bitmap

printBuf	
	ld hl,PrintBuf			;print a pair of characters
	ld b,6
	jp drawlp2
	
;*******************************************************************************	
printPlayModeIndicator
	ld de,#2988
	call setXY
	
	ld a,(PlayerFlag)
	or a
	ld d,#19			;"P"
	jr z,_playerStopped
	
	ld d,#12
	
_playerStopped
	
	ld a,(SynthMode)
	or a
	jr z,_noSynthMode
	
	ld e,#13
	jp printDE
	
_noSynthMode
	call clearPrintBuf
	ld a,d
	jp printCharLNC
	
;*******************************************************************************	
printSaveSlotIndicator			;enter with A = slot number
	ex af,af'
	ld de,#2bac
	call setXY
	
	ex af,af'
	ld d,#13			;"S"
	ld e,a
	jp printDE
	
;************************************************************************************
clearPrintBuf				;clear the print buffer in (ops)
	xor a
	ld hl,ops
	ld b,5
cpblp
	ld (hl),a
	inc l
	djnz cpblp
	ret	

;************************************************************************************	
setupPrintBuf				;set up print buffer with 5-byte bitmap (2 chars)
					;INPUT: left char in d, right char in e
					
	call clearPrintBuf		;clear the print buffer
					
	ld a,d				;load first (left) char
	ld c,2				;printing 2 characters
	exx
	ld b,0
	
setupPB1char				;init point for printing 1 char
					;char in A, B' = 0 (left) or #83 (right), C=1, print buffer must be cleared manually

	ld d,1 + (HIGH(apd_buf))	;point to font bitmaps	
_spblp
	ld hl,ops			;print buffer resides in (ops)

	ld c,a				;multiply character byte * 5
	add a,a
	add a,a
	add a,c
	add a,b				;and add right side offset as required
	ld e,a
	ld b,5
_getbitslp				;get 5 byte-length bitmaps
	ld a,(de)
	or (hl)				;and or them into the print buffer
	ld (hl),a
	inc e
	inc l				;inc hl
	djnz _getbitslp
	
	exx
	dec c				;check if all chars have been set up
	ret z				;and return if yes
	
	ld a,e				;otherwise, get second char
	exx
	ld b,#8a			;prep b with offset for shifted chars
	jr nz,_spblp			;JR NZ??? It's always NZ at this point.

;************************************************************************************
clrS					;clear main screen area
	ld d,#1f			;select horiz. pos
	ld c,7				;clearing 7 columns
_clrlp
	inc d
	ld e,#82
	call setXY

	xor a
	ld b,#3c			;clearing #3b rows
	call drawLoop

	dec c
	jr nz,_clrlp
	ret


;************************************************************************************
drawLoop				;simple draw loop (used for screen initialization)
	out (lcd_data),a
	call lcdWait3
	djnz drawLoop
	ret
	
drawlp2					;little more complex draw loop
	ld a,(hl)
	inc hl
	out (lcd_data),a
IF MODEL = TI82
	push hl				;waste some time
	pop hl
ELSE
	call lcdWait3
ENDIF
	djnz drawlp2
	ret

;************************************************************************************
lcdWait2				;waste some time till LCD controller can receive next command
	jr lcdWait3			;12t
lcdWait3

IF MODEL != TI82			;lcdWait for slow display drivers
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl
	ex (sp),hl	;152
ENDIF
	ret
	
;************************************************************************************
setXY					;set print position on screen
					;input: D=horiz.pos, E=vert.pos | out: A destroyed
	ld a,d				;set horizontal pos
	out (lcd_crt),a
	call lcdWait2
	
	ld a,e				;set vertical pos
	out (lcd_crt),a
	
setXYmod equ $+1
	jr lcdWait2			;exit via lcdWait2 - SELF-MODIFYING: swapped with a jr to lcdWait3 after initSCR


;************************************************************************************	
waitForKeyRelease			;wait for key release function.

_rdkeys
 	in a,(kbd)
 	cpl			;4	;would be nice to skip these two lines
 	or a			;4	;but somehow that doesn't work
	jr nz,_rdkeys
	
_wait
	ld b,a
	
	ld a,(LastKey)			;reset ALPHA mode if last key pressed was not ALPHA
	or a
	jr nz,_resetLK
	
	xor a
	ld (AlphaFlag),a
	
	ld de,#2888
	call setXY
	call clearPrintBuf		;clear print buffer
	call printBuf			;remove Alpha mode marker from screen
	;ld b,0				;unnecessary, printBuf returns with b=0 anyway
	jr _waitlp
	
_resetLK
	xor a				;clear LastKey flag
	ld (LastKey),a	
	
_waitlp					;waiting for keypad bounce
	ex (sp),hl
	ex (sp),hl
	ld a,(hl)
;IF MODEL = TI83 || MODEL = TI8X || MODEL = TI8XS
IF MODEL != TI82
	ld a,(hl)
ENDIF
	djnz _waitlp
	
 	ret			;5
	
;************************************************************************************
hex2char				;convert hex value to character string (using custom font)
					;input: A=hex byte | output: string in (charstring), A destroyed
	
	cp #ff				;check if we have an #ff byte
	jr z,hex2charFF

hex2charNoFF	
	push af				;preserve input byte
	and %00001111			;extract lower nibble
	ld (CharString),a		;save it
	pop af				;retrieve input byte
	rra				;shift right x4 and clear bit 4-7 to extract upper nibble
	rra
	rra
	rra
	and %00001111
	ld (CharString+1),a
	ret

hex2charFF
	ld a,#16			;replace #ff with --
	ld (CharString),a
	ld (CharString+1),a
	ret

	
hex2charU				;convert upper nibble of hex value to right-aligned char and return it in A
	cp #ff
	jr z,hex2FF
	rra				;shift right x4 and clear bit 4-7 to extract upper nibble
	rra
	rra
	rra
	and %00001111
	ret

hex2charL				;convert lower nibble of hex value to left-aligned char and return it in A
	cp #ff
	jr z,hex2FF
	and %00001111
	ret
	
hex2FF					;replace hex digit with - (dash)
	ld a,#16
	ret

	include "mem.asm"
	
;************************************************************************************
;TABLES AND INCLUDES
;************************************************************************************

baseCounterLUT			;counter values for octave 6, all other counter values are derived from this

	;dw #4000, #43CE, #47D6, #4C1C, #50A3, #556E, #5A83, #5FE4, #6598, #6BA3, #7209, #78D1
	;dw #2000, #21E7, #23EB, #260E, #2851, #2AB7, #2D41, #2FF2, #32CC, #35D1, #3905, #3C68
	;dw #14b8, #15f3, #1741, #18a3, #1a1a, #1ba8, #1d4d, #1f0b, #20e3, #22d8, #24ea, #271c
	;dw #1f6f, #214e, #2349, #2562, #279b, #29f6, #2c74, #2f19, #31e6, #34de, #3802, #3b57
	dw #1f86, #2166, #2362, #257d, #27b8, #2a14, #2c95, #2f3b, #320a, #3504, #382b, #3b82

font
	include "font.asm"
	
htlogo					;the HT2 logo
	db %11110101
	db %11110001
	db %11110101
	db %11111111
	db %11111111
	
	db %00010011
	db %10111101
	db %10111011
	db %11110001
	db %11111111
	
varmsgs					;global var names (right side of screen)

	db #10,#12			;LP
	db #13,#12			;SP(D)
	db #b,#13			;CS
	db #b,#e			;CE

notenames
	db #c,#16			;c
	db #c,#15			;c#
	db #d,#16			;d
	db #d,#15			;d#
	db #e,#16			;e
	db #f,#16			;f
	db #f,#15			;f#
	db #17,#16			;g
	db #17,#15			;g#
	db #0a,#16			;a
	db #0a,#15			;a#
	db #0b,#16			;b
	


;IF MODEL = TI83
	;org 256*(1+(HIGH($)))		;align to next page 
;ENDIF
IF ((HIGH($))<(HIGH($+30)))
	org 256*(1+(HIGH($)))		;align to next page if necessary
.WARNING kjumptab or notevals crosses page boundary, realigned to next page
ENDIF

kjumptab				;jump table for keyhandler. MAY NOT CROSS PAGE BOUNDARY!
	dw kdown
	dw kleft
	dw kright
	dw kup
	dw kpdown
	dw kpleft
	dw kpright
	dw kpup
	dw kfdown
	dw kfleft
	dw kfright
	dw kfup
ktest equ $-1
; IF ((HIGH(kjumptab))<(HIGH(ktest)))
; .ERROR kjumptab crosses page boundary
; ENDIF

notevals equ $-#a			;LUT for note name -> val conversion
	db 10	;A
	db 12	;B
	db 1	;C
	db 3	;D
	db 5	;E
	db 6	;F
	db 8	;G
ttest equ $-1
;IF ((HIGH(notevals))<(HIGH(ttest)))
;.ERROR notevals crosses page boundary
;ENDIF

;*************************************************************************************
;music player, work area, savestates

mplay
	include "player.asm"

	db 'XSAVE'
savestateLUT				;32 byte save state lookup table
	dw savestates			;DEBUG
	dw firstend-1			;DEBUG
	ds 28

savestates				;compressed savestates
	include "cpmusic.asm"
	
firstend equ $				;DEBUG
;*************************************************************************************
;memend equ $+2
	org mem_end-2
version
	db 1,2				;savestate format version


; IF ((MODEL != TI82))			; && (MODEL != TI8P))
; 		dw #0000
; ENDIF
