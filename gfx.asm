;*******************************************************************************
;GRAPHICS AND PRINTING ROUTINES
;*******************************************************************************
;*******************************************************************************
;error handling
;*******************************************************************************

errorHand0				;clear StateSelect (when entering from LOAD/SAVE)
	ex af,af'
	ld a,#16
	ld (StateSelect),a
	ex af,af'

errorHand				;error handler
					;IN: A - error code
	ld d,CHAR_E
	ld e,a
printMsg				;print a message to the message area
					;IN: DE - message	
	push de				
					
	ex af,af'			;';TODO: preserving A for what?
	setXYat #29, #b2
	ex af,af'
	
	pop de

	jp printDE
	
	
;*******************************************************************************
printMute12					;print Mute state ch1/2
	setXYat #2a, #88
	
	ld de,#0102
	ld a,(MuteState)
	rra
	jr nc,_skip
	ld d,CHAR_DASH
_skip
	rra
	jr nc,_skip2
	ld e,CHAR_DASH
_skip2
	jp printDE
	

printMute3D					;print Mute state ch3/4
	setXYat #2b, #88
	
	ld de,#030D
	ld a,(MuteState)
	rla
	jr nc,_skip
	ld e,CHAR_DASH
_skip
	rla
	jr nc,_skip2
	ld d,CHAR_DASH
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

;*******************************************************************************
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
	setXYat #21, #82
	printTwoChars CHAR_P, CHAR_T	;PT(N)
	
	setXYat #23, #82
	
	call getSeqOffset		;find pattern#
	
	ld a,(CPtn)			;load current ptn#
	cp #40				;if it's invalid (> #3f)
	jp nc,printSeqScr		;default to sequence screen

printFxScrNoInit			;init point when cycling through patterns
					
	call printChars			;print ptn#
	
	setXYat #21, #8e

	ld a,(CPtn)
	ld hl,fxptntab
	call findCurrFxPtn		;get the pattern pointer
	
	push de				;preserve pointer for printing fx params later

	ex de,hl

	call printFXP
	
	setXYat #25, #8e
	
	call printFXP
	
	setXYat #22, #8e
	
	pop hl
	inc hl
	
	call printFXP
	
	setXYat #26, #8e
	
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

;*******************************************************************************



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
	setXYat #21, #82
	printTwoChars CHAR_P, CHAR_T	;PT(N)
	printTwoChars CHAR_O, CHAR_C	;OC(T)
	
	setXYat #23, #82
	
	call getSeqOffset
	
	ld a,(CPtn)			;current ptn#
	cp #80				;if pattern# > #7f
	jp nc,printSeqScr		;default to sequence screen
	
printPtnScrNoInit			;init point when cycling through patterns
	
	call printChars
	
	ld a,(COct)			;current octave
	call printCharL

printPtnScrBasic			;init point when not reprinting ptn nr, octave etc.
	setXYat #21, #8e

	call findCurrPtn
	
	push de				;preserve pointer for printing octave #s later

	call printNoteNames
	ex de,hl
	setXYat #25, #8e
	
	ex de,hl
	call printNoteNames

	setXYat #22, #8e

	pop de				;retrieve pattern pointer
		
	call printOctaves
	
	push de
	setXYat #26, #8e
	
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
	ld a,CHAR_DASH			;print a dash
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
	ld d,CHAR_DASH			;load dashes into DE
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
	setXYat #20, #8e

	xor a				;starting with line 0
	ld b,8
	
	call printSingleLP		;print the first column
	
	setXYat #24, #8e
	
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
	setXYat #20, #82
	
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
	setXYat #21, #82		;ch1
	
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
	setXYat #22, #82
	call printSeqColL		;print lower nibbles
	
	setXYat #23, #82		;ch2

	inc hl				;increment to point to next channel
	push hl				;preserve seq. pointer (printSeqCol doesn't do this for speed reasons)
	call printSeqCol
	
	setXYat #24, #82		;ch3
	pop hl
	
	inc hl
	call printSeqColR
	setXYat #25, #82
	call printSeqColL
	
	setXYat #26, #82		;fx-ch

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
	setXYat #29, #88
	
	ld a,(PlayerFlag)
	or a
	ld d,CHAR_STOP			; <STOP> ...
	jr z,_playerStopped
	
	ld d,CHAR_P			;... or P
	
_playerStopped
	
	ld a,(SynthMode)
	or a
	jr z,_noSynthMode
	
	ld e,CHAR_S
	jp printDE
	
_noSynthMode
	call clearPrintBuf
	ld a,d
	jp printCharLNC
	
;*******************************************************************************	
printSaveSlotIndicator			;enter with A = slot number
	ex af,af'
	setXYat #2b, #ac
	
	ex af,af'
	ld d,CHAR_S			;"S"
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

	ld d,HIGH(FontLUT)		;point to font bitmaps
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

