;************************************************************************************
;the startup code
;************************************************************************************

IF MODEL = TI82 || MODEL = TI8P
checkAPD_FREE					;check if APD_BUF is in use
	ld a,(int_state)
	or a
	ret nz					;exit if it is. ->TODO: add error msg
ENDIF

IF MODEL != TI83
	rom_call(clearlcd)
ENDIF
	di
	
	push iy
	push ix
	
	ld (exitSP),sp				;preserve stack

;IF MODEL != TI8X				;setting ROM to a different page crashes TI83
IF MODEL = TI82 || MODEL = TI8P
	ld a,%10001001				;set ROM page 1
	out (rom),a
ENDIF

IF MODEL = TI8X || MODEL = TI8XS
	in a,(2)				;detect hardware
	ld b,a
	and #80					;if bit 7 is set, we have TI83+ BASIC
	jr z,reinit0
	
	xor a					
	out (#20),a				;set 6 MHz mode
	out (8),a				;disable link assist
	
	in a,(21h)
	and 3
	jr z,usbdeact				;TI84+ SE
	bit 5,b
	jr z,reinit0				;TI83+ SE

usbdeact					;TI84+/SE detected, deactivating USB to save power
	xor a
	out (#57),a
	out (#5b),a
	out (#4c),a
	ld a,2
	out (#54),a
ENDIF

reinit0
	ld a,#16
	ex af,af'
reinit
	ld hl,apd_buf				;clear APD_BUF
	ld de,apd_buf+1
	ld bc,767
	xor a
	ld (hl),a
	ldir
	
	ex af,af'
	ld (StateSelect),a

;************************************************************************************
; genDrum					;generate kick drum sample in text_mem
; 	ld d,1			;1
; 	ld a,#80		;2
; 	ld hl,text_mem		;3
; _xlp
; 	ld b,d			;1
; _lp
; 	ld (hl),a		;1
; 	inc hl			;1
; 	djnz _lp		;2
; 	
; 	inc d			;1
; 	sub #11			;2
;
; 	jr nc,_xlp		;2
; 	
; 	add a,#11		;2
; 	srl a			;2
; 	
; 	jr nz,_xlp		;2
; 	
; _exit
; 	ld (hl),a		;1
; 				;23b

generatePtnTabs					;generate pattern table and fx pattern table

	ld (_restSP),sp				;save stack pointer
	ld sp,256*((HIGH(ptntab))+1)
	
	ld de,#10
	ld hl,ptn00+(#7f*#10)			;HL = address of pnt #7f
	ld b,#80
	
_lp
	push hl					;push ptn address
	sbc hl,de				;sub #10
	djnz _lp				;rinse and repeat
	
	ld sp,fxptntab+128
	sla e					;DE = #20
	ld hl,fxptn00+(#3f*#20)
	ld b,#40
	
_lp2
	push hl
	sbc hl,de
	djnz _lp2
	
_restSP equ $+1
	ld sp,0					;restore stack pointer

;************************************************************************************
generateCounterLUT				;generate note counter value table

	ld hl,baseCounterLUT			;copy BaseCounterLUT to APD_BUF at [oct. 6]
	ld de,NoteTab+(1+12*6)*2		;table will be created on 3rd page of APD_BUF. Each note counter is a word.
						;length of LUT = silence (2 bytes) and then 7 octaves (168 bytes)
	ld bc,2*12
	ldir					;execute copy

	ld hl,NoteTab+(1+12*6)*2-1		;start reverse calculation of lower octaves at [oct. 6]-1, de already points to last byte of LUT
	ld b,12*6				;calculating 6 octaves of notes
	
_lp
	dec e					;decrement source pointer (it's initially 1 too high from previous ldir)
	xor a					;clear carry
	ld a,(de)				;load hi byte
	rra					;divide by 2
	ld (hl),a				;store
	dec e					;decrement pointers
	dec l
	ld a,(de)				;load lo byte
	rra					;divide by 2 and rotate in carry from hi byte
	ld (hl),a				;store
	dec l					;decrement target pointer
	djnz _lp				;rinse and repeat

;************************************************************************************
generateCsrLUT					;generate cursor position table
						;LUT format: byte 1 = Horiz. + set bit 7 for #f0 cursor; byte 2 = vert.
	ld c,10					;10 rows
	ld a,#87				;first row vertical offset = #87
	ld hl,CsrTab+1				;start at 2nd page of graph_mem + 1
	
_lpo						;generate vertical positions
	ld b,8					;4*2 bytes per row
_lpi
	ld (hl),a
	inc l
	inc l
	djnz _lpi
	add a,6
	dec c
	jr nz,_lpo
	
	ld b,10
	ld l,0					;reset LUT pointer

_lp2
	ld a,#21
	ld (hl),a				;21
	inc l
	inc l
	add a,#81				;inc a \ add a,#80
	ld (hl),a				;22+80
	inc l
	inc l
	inc a
	ld (hl),a				;23+80
	inc l
	inc l
	sub #80					;A-80
	ld (hl),a				;23
	inc l
	inc l
	inc a
	ld (hl),a				;24
	inc l
	inc l
	add a,#81
	ld (hl),a				;25+80
	inc l
	inc l
	inc a
	ld (hl),a				;26+80
	inc l
	inc l
	sub #80
	ld (hl),a				;26
	inc l
	inc l
	djnz _lp2

_generatePtnCsr					;generating pattern screen cursor positions at offset #50*2
	ld d,#21
	call fillTable
	ld d,#22+#80
	call fillTable
	ld d,#25
	call fillTable
	ld d,#26+#80
	call fillTable
						
_generateFxCsr					;generate fx screen cursor positions at text_mem
	ld hl,CsrTab2
	ld d,#21+#80
	call fillTable
	ld d,#21
	call fillTable
	ld d,#22+#80
	call fillTable
	ld d,#22
 	call fillTable
	ld d,#25+#80
	call fillTable
	ld d,#25
	call fillTable
	ld d,#26+#80
	call fillTable
	ld d,#26				;THIS CAUSES THE "LOW CONTRAST BUG"
	call fillTable				;8 bytes is ok, 10 is too much - because it starts to write at #8000, d'uh
	
	jr unpackFont
	
fillTable					;fill a chunk of the cursor position LUTs
	ld b,8
	ld a,#93
_lp
	ld (hl),d
	inc l
	ld (hl),a
	inc l
	add a,6
	djnz _lp
	ret		

;************************************************************************************
unpackFont				;unpack the font
	ld hl,font
	ld de,FontLUT			;unpacked font will be created in APD_BUF (TI82 at #8300)
	push de				;preserve pointer to unpacked font
	
	ld b,cmprFontSize		;unpacking all the font bytes
	or a				;clear carry
	
unpackFontLP
	ld a,(hl)			;load compressed char byte
	and %11110000
	ld (de),a			;save unpacked byte
	inc de				;increment pointer to unpacked font
	ld a,(hl)			;load compressed char byte again
	add a,a				;shift left 4 bits
	add a,a
	add a,a
	add a,a
	ld (de),a
	inc de
	inc hl
	djnz unpackFontLP
		
	dec de				;when uneven amount of chars, last unpacked byte is irrelevant and can be overwritten. TODO: Use IF/ENDIF
	pop hl				;retrieve pointer to unpacked font
	
					;create right-shifted chars from unpacked font
	
	ld b,charNumShifted*5		;amount of chars to shift, use only the necessary ones
	or a				;clear carry
	
shiftFontLP
	ld a,(hl)			;load unshifted pixel row
	rra				;shift right 4 pixels
	rra
	rra
	rra
	ld (de),a			;save unpacked byte
	inc de
	inc hl
	djnz shiftFontLP


initSCR					;draw the basic screen layout
	ld a,#f2			;modify setXY code to use longer wait lp
	ld (setXYmod),a

	setXYat #20, #80		;select column 0, row 0
	
	ld a,7				;cursor direction right
	out (lcd_crt),a
	call lcdWait3

	ld b,#c				;12*8=96 pixel
	ld a,#ff			;data to write
	
	call drawLoop			;draw a line on top of the screen
	
	setXYat #29, #81
	
	ld a,5				;cursor direction down
	out (lcd_crt),a
	call lcdWait3

	ld b,a				;ld b,5
	ld c,a				;preserve 5 in c because we'll need that constant a few more times
	ld a,#ff
	call drawLoop			;draw black block next to the logo
	
	setXYat #28, #81
	
	ld b,c				;ld b,5
	ld a,%00101111

_drawlp					;draw fancy block left of the black block
	out (lcd_data),a
	call lcdWait3
	sra a
	djnz _drawlp
	
	setXYat #2a, #81		;draw HT2 logo in the top right corner
	
	ld hl,htlogo
	ld b,c				;ld b,5
	call drawlp2
	
	setXYat #2b, #81
	
	ld b,c				;ld b,5
	call drawlp2

	ld a,#f4			;modify setXY code to use shorter wait lp
	ld (setXYmod),a


printVarNames				;create global var names on the right side

	setXYat #2a, #ac		;reset RowPlay indicator
	call clearPrintBuf		;clear print buffer
	call printBuf

	ld a,(StateSelect)
	call printSaveSlotIndicator

	call printPlayModeIndicator	;print STOP char
	
	ld hl,varmsgs			;load chars			
	ld b,4				;4 messages to print

_rdlp	
	ld d,(hl)
	inc hl
	ld e,(hl)
	push hl
	push bc

	call printDE			;print the chars held in DE
	
	pop bc
	pop hl
	inc hl
	djnz _rdlp

	ld a,CHAR_D			;print a "D" (as in usr Drum)
	call printCharL
	
	ld a,1
	ld (AutoInc),a
	
	printTwoChars CHAR_A, CHAR_0	;print AutoInc indicator
	
p123D					
	ld a,#9f			;reset mute states
	ld (mute1),a
	ld (mute2),a
	ld (mute3),a
	ld a,#d2			
	ld (muteD),a
	
	call printMute12		;print channel active/mute info (123D)
	call printMute3D


;end of screen initialization
;************************************************************************************

printVars				;print global var names
	
	ld a,(looprow)			;temporary set loop point
	call printCharsNoFF		;print them
	
	ld a,(speed)			;load global speed
	call printCharsNoFF		;print
	
	ld a,(CPS)
	call printCharsNoFF		;print
	
	ld a,(CPE)
	call printCharsNoFF		;print
	
	ld a,(usrDrum)			;print lo byte of usr drum pointer
	call printChars
	
	setXYat #2a, #a6
	
	ld a,(usrDrum+1)		;print hi byte of usr drum pointer
	call printChars

; checkSavestate
;  	ld a,(version)
;  	or a
;  	jr z,ldok
;  	ld a,#6
;  	call errorHand
ldok


	call printSeqScr		;print the main (sequence) screen

	call initSeqCsr			;init cursor

