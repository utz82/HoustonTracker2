;************************************************************************************
;the key handler
;************************************************************************************
inputSlotNr
	ld a,#ef
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	jr nc,ik0
	rra
	jr nc,ik1
	rra
	jr nc,ik4
	rra
	jr nc,ik7

	ld a,#f7
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	rra
	jr nc,ik2
	rra
	jr nc,ik5

	ld a,#fb				;group (-)/3/6/9/)/G/VARS
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	rra
	jr nc,ik3
	rra
	jr nc,ik6
	jr inputSlotNr
	
ik0
	ld b,0
	jr kSlotSelect
ik1
	ld b,1
	jr kSlotSelect
ik2
	ld b,2
	jr kSlotSelect
ik3
	ld b,3
	jr kSlotSelect
ik4
	ld b,4
	jr kSlotSelect
ik5
	ld b,5
	jr kSlotSelect
ik6
	ld b,6
	jr kSlotSelect
ik7
	ld b,7


kSlotSelect				;print state selection, and update it
	ld de,#2bb2
	call setXY
	
	ld a,b
	
	ld (StateSelect),a		;store selected state
	
	jr reprint
	;call printCharL			;reprint current 
	
	;xor a
	;ld (InputType),a		;reset input type
	
	;call printCsr			;print cursor

	;jp waitForKeyRelease
	
;**************

inputSingle				;aka set current octave
	
	ld de,#2388
	call setXY
	
	ld a,b
	cp 7				;check if input digit is in range 0..6
	ret nc				;ignore keypress if it isn't
	
	ld (COct),a			;store new current oct

reprint	
	call printCharL			;reprint current oct
	
	xor a
	ld (InputType),a		;reset input type
	
	jp kdirskip2			;print cursor & wait for key release
	
inputDouble				;input a double digit hex value
					;IN: set iPrintPos and iUpdatePos				
	push af
					
iPrintPos equ $+1
	ld de,0
	call setXY

iUpdatePos equ $+1
	ld hl,0
	
	pop af
	rra
	jr c,inputDoubleLow		;if input val=6, we're inputting the low nibble
	
	ld a,6				;the next hex input should be a low nibble
	ld (InputType),a
	
	ld a,b				;load input digit
	push af

	add a,a
	add a,a
	add a,a
	add a,a

	ld (hl),a
	
	pop af
	call printCharL			;print it as upper nibble

iDUpdate
	jp waitForKeyRelease
	;ret

inputDoubleLow
	xor a				;reset input type
	ld (InputType),a
	
	ld a,b
	or (hl)
	ld (hl),a
	
	call printCharsNoFF
	
	ld a,(CScrType)			;check what type of cursor to print
	cp 2
	jr nz,_normal
	call printCsr2
	jr wordSwitch-1
_normal
	call printCsr
	
wordSwitch equ $+1
	ld a,0
	or a
	jr z,iDUpdate
	
	xor a
	ld (wordSwitch),a

	call waitForKeyRelease
	ld hl,usrDrum
	ld de,#2ba6
	call setXY
	jp kSetBFull	
	

inputWord
	ex af,af'
	ld a,1
	ld (wordSwitch),a
	ex af,af'
	jr inputDouble


kHexInp					;handle hex digit input

	ld b,a				;preserve the input value
	
	or a				;clear carry
	ld a,(InputType)		;determine input type
	rra
	jp c,inputSingle
	rra
	jp c,inputDouble
	rra
	jp c,inputWord
	
	ld a,(CScrType)			;check screen type
	or a
	jp nz,kHexNoSeq
	
	call getSeqOffset		;now seq. pointer in HL, current ptn# in (CPtn)
	
	ld a,(AlphaFlag)		;check for Alpha mode
	or a
	jr z,_noalpha
	ld a,b				;check if input char was 0
	or a
	jr nz,_noalpha			;if both Alpha mode and 0, delete pattern # at cursor
	
	ld a,#ff
	ld (hl),a
	
	jp pagescroll+3
	;ld a,(CsrPos)
	;ld (OldCsrPos),a
	;call printSeqScr0
	;call printCsr
	;jp waitForKeyRelease

_noalpha	
	ld a,(CsrPos)			;check whether we need to shift the digit to upper nibble
	rra
	ld a,b				;retrieve input value
	jr c,_noShift			
	add a,a				;shift left 4 bits
	add a,a
	add a,a
	add a,a
	ld b,a
	ld a,(CPtn)			;load current ptn value
	and %00001111			;delete upper nibble
	jr _byteUpdate

_noShift			
	ld a,(CPtn)			;load current ptn value
	and %11110000			;delete lower nibble
	
_byteUpdate			
	or b				;combine with input value
	ld e,a				;preserve in E
	
	ld d,#7f			;make sure ptn# does not exceed #7f
	ld a,(CsrPos)			;check column type
	and 7				;mask out bits that are irrelevant to horizontal pos
	cp 6
	jr c,_skip			;if column > 5
	ld d,#3f			;we're setting fx ptn# - make sure it doesn't exceed #3f
_skip	
	ld a,e
	and d				
	ld (hl),a			;and store it at song pointer
	ld (CPtn),a

	call setPrintPos	
	call updateSeqScr
	
	ld a,(AutoInc)			;check AutoInc mode
	or a
		
	;jp z,kright			
	jp z,kRnoalpha			;if AutoInc mode is on, update cursor as if RIGHT key had been pressed
	;jp printCsr			;else, simply reprint the cursor at the current position
	jp kdirskip2


updateSeqScr				;update an entry on the sequence screen
	ld a,(CsrPos)			;load cursor pos
	and 3				;check what type of column is being printed
	cp 0				;and select the print function accordingly
	jr z,uRight
	cp 1
	jr z,uLeft

uBoth
	ld a,(CPtn)
	jp printChars
	;ret
	
uRight
	ld a,(CPtn)
	push de
	call printCharR
	pop de
	inc d
	call setXY
	ld a,(CPtn)
	jp printCharL
	;ret
	
uLeft
	ld a,(CPtn)
	push de
	call printCharL
	pop de
	dec d
	call setXY
	ld a,(CPtn)
	jp printCharR
	;ret

;************************************************************************************
kHexNoSeq				;handling input on ptn screen
					;IN: B=hex digit
	dec a				;check if we're on an fx ptn screen
	jp nz,kHexFX

kHexPtn
	call findCurrPtn		;get ptn pointer into DE
	ld h,0
	ld a,(CsrPos)			;get cursor position
	ld c,a				;preserve it in C
	cpl				;convert it to ptn data offset
	and #10
	rrca
	ld l,a
	ld a,c
	and #7
	add a,l
	ld l,a

	add hl,de			;position to edit now referenced by HL
	push hl				;a08a

	ld a,c				;retrieve cursor position and determine if input is a note name or an octave
	and 8
	ld a,b				;retrieve input digit
	jr nz,inputOct
	
inputNote
	or a
	jr z,_exitlp

	cp #a				;check if input digit is A..G... handle G seperatly?
	jp c,ignoreKeypress		;ignore keypress if it isn't
	
	ld c,a				;preserve input digit in C
	
	ld hl,notevals			;load note val conversion LUT pointer
	add a,l				;add offset
	ld l,a

	ld a,(COct)			;fetch current octave
	ld b,a
	inc b

	ld a,(AlphaFlag)		;check if ALPHA mode is active
	or a
	jr z,_skip			;if it is
	
	ld a,#b				;check if input note was a B
	sub c
	jr z,_skip			;skip raising halftone if it was
			
	ld a,1				;add a half-tone (TODO: check if it's a valid halftone - or ignore and always go a halftone higher, also fine)
_skip
	add a,(hl)			;get base note val
	ld c,a				;preserve it in C
_lp	
	dec b
	jp z,_exitlp
	add a,12
	jr _lp
	
_exitlp					;note val now in A
	pop hl
	ld (hl),a			;update note byte
	or a
	jr z,_reprintDash
	
_reprint
	call setPrintPos
	
	push de

	ld a,c				;restore base note val
	dec a				;note names are offset by 1, correct it
	ld hl,notenames			;point to note name LUT
	add a,a				;A=A*2
	ld e,a
	ld d,0
	add hl,de			;add offset to LUT pointer
	
	ld d,(hl)			;get string into DE
	inc hl
	ld e,(hl)
	
	call printDE
	pop de				;retrieve print pos
	inc d				;inc horiz. pos by 1
	call setXY
	
	ld a,(COct)			;print current octave
	call printCharL
	
	ld a,(RowPlay)
	or a
	call nz,rowPlay

_skipRP
	ld a,(AutoInc)
	or a	
	jp z,kpdown			;and update cursor as if DOWN key has been pressed
;	call printCsr
;	jp waitForKeyRelease
	jp kdirskip2


_reprintDash				;print dashes if new note byte = 0
	call setPrintPos
	push de
	
	ld de,#1616
	call printDE
	
	pop de
	inc d
	call setXY
	
	call clearPrintBuf
	ld a,#16
	call printCharLNC
	
	;jp kpdown
	jr _skipRP

	
inputOct
	pop hl			
	cp 7				;check if input digit is 0..6
	jp nc,ignoreKeypress+1		;ignore keypress if it isn't
	push bc
	
	ld a,(hl)			;load current note val
	or a				;if no note is set
	jp z,ignoreKeypress		;ignore keypress
	
	call divNoteVal			;returns current octave in B, base note val in C
	
	ld a,c				
	pop bc
	ld c,b				;preserve input digit (new octave) in C
	inc b

_lp	
	dec b
	jr z,_skip
	add a,12			;for each octave >0, add 12 to base note val
	jr _lp
_skip
	inc a				;note vals are offset by 1
	ld (hl),a			;store new load value

	call setPrintPos
	
	ld a,c				;retrieve input digit (new oct)
	
	call printCharL			;print it
	
	jp kpdown			;and update cursor as if DOWN key has been pressed


;************************************************************************************	
kHexFX					;handle hex input on fx ptn scr
					;IN: B=hex digit
					
	ld a,(CPtn)			;locate current fx ptn in mem
	ld hl,fxptntab
	call findCurrFxPtn		;ptn pointer now in DE
	
	ld hl,0
	ld a,(CsrPos)			;read cursor position
	ld c,a				;backup in C
	bit 4,a				;convert cursor position into ptn data offset
	jr z,_skip3
	inc l
_skip3
	and #20
	rrca
	add a,l
	ld l,a
	ld a,c
	and #7
	rlca
	add a,l
	ld l,a
	
	add hl,de			;byte to edit now referenced by HL
	
	ld a,c				;restore csrpos
	and %00001000			;determine whether input is hi or low nibble
	ld a,b				;restore input val
	jr nz,_skip			;if Z then it's a hi nibble
	add a,a				;shift input value left 4 bits
	add a,a
	add a,a
	add a,a
	ld b,a
	ld a,(hl)
	and %00001111			;clear upper nibble
	jr _set
_skip	
	ld a,(hl)
	and %11110000			;clear lower nibble
_set
	or b				;combine low and hi nibble
	ld (hl),a	
	
	ld c,a
	ld hl,CsrTab2
	call setPrintPosFx
	ld a,c
	
	call printCharsNoFF
	
	ld a,(AutoInc)
	or a
	jp z,kfdown

	call printCsr2
	jp waitForKeyRelease

ignoreKeypress
	pop hl				;clear stack
	;ret				;and back to where we came from
	jp waitForKeyRelease

;************************************************************************************
setPrintPos				;set print position based on current cursor pos
					;IN: nothing | OUT: print pos set and in DE; HL, A destroyed
	ld hl,CsrTab
setPrintPosFx				;entry point for FX patterns, HL must be set to CsrTab2
	ld a,(CsrPos)
	add a,a
	add a,l
	ld l,a
	ld a,(hl)
	and %01111111			;mask out bit 7 since cursor type is irrelevant at this point
	ld d,a
	inc l
	ld a,(hl)
	sub 5				;actual horiz. pos = CsrPos - 5
	ld e,a
	jp setXY			;using the ret from setXY

;************************************************************************************
kfdown
	call delCsr2			;delete cursor
	ld a,(CsrPos)
	inc a				;jump to next row
	ld d,a
	and %00000111
	or a	
	ld a,d
	jr nz,kdirskipF			;if bottom row reached
	add a,#18			;jump to top of same column, 2nd half
	cp #40
	jr c,kdirskipF			;if bottom row on 2nd half reached
	sub #40				;jump to top of same colum, 1st half
	
kdirskipF
	ld (CsrPos),a
	call printCsr2
	jp waitForKeyRelease
	;ret

kfleft
	ld a,(AlphaFlag)		;check Alpha mode
	or a
	jr z,_noalpha
					;if Alpha is on, cycle through patterns
	ld de,#2382			;TODO: optimize | set printing pos
	call setXY
	
	ld a,(CPtn)			;increment "current pattern" value
	dec a
	and #3f				;make sure we don't go above #7f
	ld (CPtn),a

	jp printFxScrNoInit		;and print the new pattern
	
_noalpha
	call delCsr2			;delete cursor
	ld a,(CsrPos)
	sub 8				;jump to previous column
	jr nc,kdirskipF			;if already in the 1st column	
	add a,#40			;wrap to last column
	jr kdirskipF

kfright
	ld a,(AlphaFlag)		;check Alpha mode
	or a
	jr z,_noalpha
					;if Alpha is on, cycle through patterns
	ld de,#2382			;TODO: optimize | set printing pos
	call setXY
	
	ld a,(CPtn)			;increment "current pattern" value
	inc a
	and #3f				;make sure we don't go above #7f
	ld (CPtn),a

	jp printFxScrNoInit		;and print the new pattern
	
_noalpha
	call delCsr2			;delete cursor
	ld a,(CsrPos)
	add a,8				;jump to next column
	cp #40
	jr c,kdirskipF			;if already in the last column
	sub #40				;wrap to first column
	jr kdirskipF

kfup
	call delCsr2
	ld a,(CsrPos)
	dec a
	ld d,a
	and %00000111
	cp #7
	ld a,d
	jr nz,kdirskipF
	sub #18				;#ff-#18 doesn't set carry,
	or a				;so we need to check if result was negative
	jp p,kdirskipF			;like that
	add a,#40
	jr kdirskipF


kpdown
	call delCsr			;delete cursor
	ld a,(CsrPos)
	inc a				;jump to next row
	ld d,a
	and %00000111
	or a				;TODO: redundant?	
	ld a,d
	jp nz,kdirskip			;if bottom row reached
	add a,#8			;jump to top of same column, 2nd half
	cp #70
	jp c,kdirskip			;if bottom row on 2nd half reached
	sub #20				;jump to top of same colum, 1st half
	jp kdirskip

kpleft
	ld a,(AlphaFlag)		;check Alpha mode
	or a
	jr z,_noalpha
					;if Alpha is on, cycle through patterns
	ld de,#2382			;TODO: optimize | set printing pos
	call setXY
	
	ld a,(CPtn)			;increment "current pattern" value
	dec a
	and #7f				;make sure we don't go above #7f
	ld (CPtn),a

	jp printPtnScrNoInit		;and print the new pattern
	
_noalpha
	call delCsr
	ld a,(CsrPos)
	sub 8
	cp #50
	jp nc,kdirskip
	add a,#20
	jp kdirskip

kpright
	ld a,(AlphaFlag)		;check Alpha mode
	or a
	jr z,_noalpha
					;if Alpha is on, cycle through patterns
	ld de,#2382			;TODO: optimize | set printing pos
	call setXY
	
	ld a,(CPtn)			;increment "current pattern" value
	inc a
	and #7f				;make sure we don't go above #7f
	ld (CPtn),a

	jp printPtnScrNoInit		;and print the new pattern

_noalpha					;do normal cursor movement	
	call delCsr
	ld a,(CsrPos)
	add a,8
	cp #70
	jp c,kdirskip
	sub #20
	jp kdirskip

kpup
	call delCsr
	ld a,(CsrPos)
	dec a
	ld d,a
	and %00000111
	cp #7
	ld a,d
	jp nz,kdirskip
	sub #8
	cp #50
	jp nc,kdirskip
	add a,#20
	jp kdirskip

kup
 	ld a,(CsrPos)
 	ld (OldCsrPos),a
	ld a,(AlphaFlag)
	or a
	jr z,kuskip
	xor a
	ld (FirstLineMS),a
	call printSeqScr
	jp kdirskip2
	
kuskip	
	call delCsr				;delete cursor
	ld a,(CsrPos)
	sub 8					;subtract 8 from cursor pos
	jp nc,kdirskip				;and that's it, unless cursor is about to go off-screen
	
	ld a,(FirstLineMS)			;in that case, check what's the first line of sequence data on screen
	or a
	jp z,kdirskip2				;if it's 0, don't change anything and that's it
	
	dec a					;else, decrement FirstLine
	;ld (FirstLineMS),a			;and store it
	;call printSeqScr0			;reprint sequence screen (without setting screen type / kdir jump table offset)
	;jp kdirskip2
	jr seqScrReprint
	
kright
	ld a,(AlphaFlag)			;check for Alpha mode
	or a
	jr z,kRnoalpha
	ld a,(FirstLineMS)			;if Alpha mode is set, scroll 1 page (10 lines)
	add a,#a				;add 10 to FirstLine
pagescroll0
	cp #f7
	jr c,pagescroll				;if result is >#f6
	ld a,#f6				;set FirstLine to #f6
pagescroll
	ld (FirstLineMS),a
reprintX
	ld a,(CsrPos)
	ld (OldCsrPos),a
reprintY
	call printSeqScr0
	call printCsr
	jp waitForKeyRelease
	
kRnoalpha	
	call delCsr
	ld a,(CsrPos)
	inc a
	cp #50					;check if cursor pos limit reached
	jr nz,kdirskip
	;xor a
	ld a,#48				;if yes, move cursor to beginning of the same row
	ld (OldCsrPos),a
	jr kdskip+3				;and scroll screen

kdown
 	ld a,(CsrPos)
 	ld (OldCsrPos),a
	ld a,(AlphaFlag)
	or a
	jr z,kdskip

	ld a,#f6
	ld (FirstLineMS),a
	call printSeqScr
	jr kdirskip2
	
kdskip
	call delCsr
	ld a,(CsrPos)
	add a,8
	cp #50
	jr c,kdirskip

	ld a,(FirstLineMS)			;need to implement scroll/wrap here
	cp #f6					;#100 - 0a
	jr z,kdirskip2
	
	inc a
seqScrReprint
	ld (FirstLineMS),a
	call printSeqScr0
	jr kdirskip2

kleft
	ld a,(AlphaFlag)			;check for Alpha mode
	or a
	jr z,_noalpha
	ld a,(FirstLineMS)			;if Alpha mode is set, scroll 1 page (10 lines)
	sub #a
	jr pagescroll0

_noalpha	
	call delCsr
	ld a,(CsrPos)
	sub 1
	jr nc,kdirskip
	;ld a,#4f
	ld a,7
	ld (OldCsrPos),a
	jp kuskip+3

	
kdirskip
	ld (CsrPos),a
kdirskip2
	call printCsr
kdirskip3
	jp waitForKeyRelease	
	;ret

kdir						;determine which direction key has been pressed
	rra
	jr nc,_kp
	inc d
	jp kdir
_kp
	ld h,HIGH(kjumptab)			;set hi byte of jump table pointer
kdirswitch equ $+1				;switch for changing the response to keypress according to ptn type
	ld a,LOW(kjumptab)
	add a,d					;add D*2
	add a,d
	ld l,a					;set lo byte of jump table pointer
	ld e,(hl)				;get jump value into DE
	inc l
	ld d,(hl)
	ex de,hl				;swap jump value into HL
	
	jp (hl)					;and jump


;************************************************************************************
keyhand						;the main keyhandler

	ld a,#fe				;group dirpad
	out (kbd),a
IF MODEL != TI82
	nop
	nop
ENDIF
	ld e,#ff
	ld d,0
	in a,(kbd)
	cp e
	jr nz,kdir

	ld a,#fd				;group ENTER/+/-/*/div/CLEAR
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rra					;ENTER
	jp nc,kenter
	rra
	jp nc,kplus				;+
	rra
	jp nc,kminus				;-
	rra
	jp nc,kmult				;*
	rra
	jp nc,kdiv				;/
	rra
	jp nc,kpot				;^
	rra
	jp nc,kclear				;CLEAR

	ld a,#fb				;group (-)/3/6/9/)/G/VARS
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rra
	jp nc,kneg
	rra
	jp nc,k3
	rra
	jp nc,k6
	rra
	jp nc,k9
	rra
	jp nc,kcbracket
	rra
	jp nc,kG
	rra
	jp nc,kvars	
	
	ld a,#f7
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rra
	jp nc,kdot
	rra
	jp nc,k2
	rra
	jp nc,k5
	rra
	jp nc,k8
	rra
	jp nc,kobracket
	rra
	jp nc,kF
	rra
	jp nc,kC
	rra
	jp nc,kstat
	
	ld a,#ef
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rra
	jp nc,k0
	rra
	jp nc,k1
	rra
	jp nc,k4
	rra
	jp nc,k7
	rra
	jp nc,kcomma
	rra
	jp nc,kE
	rra
	jp nc,kB
	rra
	jp nc,kxto
	
	ld a,#df
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rla
	jr nc,kalpha
	rla
	jr nc,kA
	rla
	jr nc,kD
	rla
	jr nc,kxsq
	rla
	jr nc,klog
	rla
	jr nc,kln
	rla
	jr nc,ksto
	
	ld a,#bf
	out (kbd),a
	;nop
	;nop
	key_delay
	in a,(kbd)
	rra
	jp nc,kgraph
	rra
	jp nc,ktrace
	rra
	jp nc,kzoom
	rra
	jp nc,kwindow
	rra
	jp nc,kyeq
	rra
	jp nc,k2nd
	rra
	jp nc,kmode
	rra
	jp nc,kdel
	
	ld a,(AlphaFlag)			;check ALPHA mode
	or a
	ret z					;exit keyhandler if not set (ignoring ON key)
	
	in a,(kon)				;check ON key
	and %00001000
	jp z,exit
	ret					;


;************************************************************************************
kalpha
	ld de,#2888
	call setXY

	ld a,(AlphaFlag)	
	xor #a0
	ld (AlphaFlag),a			;set AlphaFlag
	ld (LastKey),a				;set LastKey

	or a
	jr z,_skip
		
	call printCharR				;print Alpha mode marker
_skip0	
	jp waitForKeyRelease
	;ret

_skip
	call clearPrintBuf			;clear print buffer
	call printBuf				;remove Alpha mode marker from screen
	jr _skip0	

kA
	ld a,#0a
	jp kHexInp
kD
	ld a,#0d
	jp kHexInp

kxsq						;set loop point
	ld hl,looprow
	ld de,#2b8e
	jr kBSet

klog
	ret
kln						;set BS
	ld hl,CPS
	ld de,#2b9a
	jr kBSet

ksto						;set BE
	ld hl,CPE
	ld de,#2ba0

kBSet
	call setXY
						
	ld a,(AlphaFlag)
	or a
	jr nz,kSetBFull				;if ALPHA mode is set, do a regular set BT

kSetBlock
	ld a,(CScrType)				;check screen type
	or a
	ret nz					;for now, ignore keypress if not on main screen TODO: implement alternative for ptn screens
	
	ld a,(FirstLineMS)			;calculate current line
	ld b,a
	ld a,(CsrPos)
	rra					;on-screen row# = (CsrPos/8 AND %00001111)
	rra
	rra
	and #f
	add a,b					;absolute row# in sequence = first on-screen row# + current on-screen row#
	ld (hl),a
	
	call printChars
exitthis
	jp waitForKeyRelease
	
kSetBFull
	ld a,2					;set InputType to signal 2-digit hex input
	ld (InputType),a
	;ld hl,CPT
	ld (iUpdatePos),hl			;set position in memory to update
	ex de,hl
	ld (iPrintPos),hl			;set print position
	
	call clearPrintBuf
	call printBuf
	
	ld a,(CScrType)				;check what type of cursor to delete
	cp 2
	jp nz,delCsr
	jp delCsr2
;*****	
kgraph						;set current Octave

	ld a,(CScrType)				;check screen type
	dec a
	ret nz					;if not on a ptn screen, ignore keypress				
	ld a,1
	ld (InputType),a
	
	ld de,#2388
	call setXY
	call clearPrintBuf
	call printBuf
	
	call delCsr				;temporarily disable cursor
	jp waitForKeyRelease
	
ktrace
	ld a,(AlphaFlag)
	or a
	jr nz,setUsrDrumHi
	
	ld hl,speed
	ld de,#2b94
	call setXY
	jp kSetBFull

setUsrDrumHi
	ld hl,usrDrum+1
	ld de,#2aa6
	call setXY
	ld a,4
	jp kSetBFull+2
	;jp kSetBFull
	
; setUsrDrumLo					;TODO: dead code?
; 	ld hl,usrDrum
; 	ld de,#2ba6
; 	call setXY
; 	jp kSetBFull

kzoom
	ret
kwindow						;delete save slot / clear current tune

	ld a,(PlayerFlag)			;check if player is running - actually unnecessary, should be save to save while player is running
	or a
	ret nz					;if it is, ignore command
	
	ld a,(AlphaFlag)			;check alpha flag
	or a
	jr z,_zap
	call delSlot
	
	jp waitForKeyRelease
	;jp nz,delSlot				;if Alpha mode is active, delete save slot
						;else, clear current tune
_zap	
	ld de,#0c0a				;print "CA" message
	call printMsg
	call confirmAction
	jp c,exitthis

	pop hl
	call zap
	
	jp reinit0	

kyeq						;load
	ld a,(PlayerFlag)			;check if player is running - actually unnecessary, should be save to save while player is running
	or a
	ret nz					;if it is, ignore command
	ld de,#100d				;LD message
	ld a,(AlphaFlag)
	ld iyh,a
	or a
	jr z,_skip
	ld de,#130a				;SA message
_skip	
	call printMsg
	call stateSelect
	call confirmAction
	jp c,exitthis
	
	;ld a,(AlphaFlag)
	ld a,iyh
	or a
	jp nz,save
	
	pop hl					;pop return address from stack
	call load				;load song

	ld a,(StateSelect)
	ex af,af'
	jp reinit				;reinit HT2

k2nd
	ld a,(CScrType)				;check if we're on a pattern screen
	or a
	jr nz,_retPtnS
	call clrS				;clear screen
	ld a,(CsrPos)				;determine whether to print normal or fx pattern
	ld (OldCsrPos),a
	and %00000110
	cp 6
	jp nz,printPtnScr
	jp printFxScr

_retPtnS
	call printSeqScr
	call initSeqCsr
	jp waitForKeyRelease
	;ret
kmode						;switch AutoInc/RowPlay mode
	ld a,(AlphaFlag)
	or a
	jr nz,_toggleRP
	
	ld de,#29ac
	call setXY
	
	ld a,(AutoInc)
	xor 1
	ld (AutoInc),a
	
	xor 1
	ld e,a
	ld d,#a
	
	call printDE
	
	jp waitForKeyRelease

_toggleRP
	ld de,#2aac
	call setXY
	
	ld a,(RowPlay)
	cpl
	ld (RowPlay),a
	
	or a
	jr z,_skipx
	call clearPrintBuf
	ld a,#12
	call printCharLNC	
	jp waitForKeyRelease
	
_skipx
	call clearPrintBuf			;clear print buffer
	call printBuf
	jp waitForKeyRelease	

kdel
	ret
k0
	xor a
	jp kHexInp
k1
	ld a,1
	jp kHexInp
k4
	ld a,4
	jp kHexInp
k7
	ld a,7
	jp kHexInp

kcomma						;mute ch1
	ld a,(AlphaFlag)
	or a
	jr nz,_unmuteAll
	ld a,(MuteState)			;toggle flag bit 0
	xor 1
	ld (MuteState),a
	ld a,(mute1)				;toggle mute switch
	;xor #28
	xor #8
	ld (mute1),a
	call printMute12
	jp waitForKeyRelease

_unmuteAll
	xor a
	ld (MuteState),a
	ld a,#9f
	ld (mute1),a
	ld (mute2),a
	ld (mute3),a
	ld a,#d2				;jp nc = #d2, jp = #c3
	;ld (mute1),a
	ld (muteD),a
	call printMute12
	call printMute3D
	jp waitForKeyRelease


kE
	ld a,#0e
	jp kHexInp

kB
	ld a,#0b
	jp kHexInp
	
kxto						;transpose
	ld a,(CScrType)
	dec a					;checking if we're currently on a note pattern screen
	jp nz,waitForKeyRelease

	call findCurrPtn			;pattern pointer now in DE
	
	ld b,#10				;16 notes to transpose
	
	ld a,(AlphaFlag)
	or a
	jr nz,_transposeDown

_transposeUp
	ld a,(de)
	or a					;check for rest (#00)
	jr z,_notranspose
	inc a					;transpose up by 1 halftone
	cp #55					;check for upper limit (#00)
	jr nz,_notranspose
	xor a					;replace with if upper limit crossed
	
_notranspose
	ld (de),a
	inc de
	djnz _transposeUp
	
_done	
	jp printPtnScrBasic	
	
	
_transposeDown
	ld a,(de)
	dec a
	cp #ff
	jr nz,_notranspose2
	xor a
	
_notranspose2
	ld (de),a
	inc de
	djnz _transposeDown
	
	jp printPtnScrBasic
	

	;ret
kdot	
	ret
k2
	ld a,2
	jp kHexInp
k5
	ld a,5
	jp kHexInp	
k8
	ld a,8
	jp kHexInp

kobracket
	ld a,(AlphaFlag)
	or a
	jr nz,kMuteD

	ld a,(MuteState)			;toggle flag bit 1
	xor 2
	ld (MuteState),a
	ld a,(mute2)
	xor #8					;toggle between jr nc and unconditional jr
	ld (mute2),a
	call printMute12
	jp waitForKeyRelease

kMuteD
	ld a,(MuteState)			;toggle flag bit 7
	xor #80
	ld (MuteState),a
	ld a,(muteD)
	xor #11
	ld (muteD),a
	call printMute3D
	jp waitForKeyRelease	



kF
	ld a,#0f
	jp kHexInp
kC
	ld a,#0c
	jp kHexInp
kstat
kneg					;play from current row
	ld a,(PlayerFlag)
	or a
	jp nz,waitForKeyRelease
	
	ld (exitPlayerSP),sp		;store SP for exiting player
	inc a				;set player flag, A=1
	ld (PlayerFlag),a
	
	call printPlayModeIndicator	
	call waitForKeyRelease
	
	xor a
	ld h,a
	ld d,a
	ld a,(CScrType)			;if on seq.screen
	or a
	ld a,(CsrPos)			;derive current sequence line from (CsrPos)
	jr z,_skip
	ld a,(OldCsrPos)		;else, derive it from (OldCsrPos)
_skip	
	call findCurrLineNoSeq		;find current line in sequence
	ex de,hl			;get pointer into DE
	
	call initrp			;call player
	jp exitplayer
	
k3
	ld a,3
	jp kHexInp
k6
	ld a,6
	jp kHexInp
k9
	ld a,#09
	jp kHexInp
	
kcbracket
	ld a,(AlphaFlag)
	or a
	jr nz,_muteAll
	ld a,(MuteState)			;toggle flag bit 6
	xor #40
	ld (MuteState),a
	ld a,(mute3)				;toggle between sbc a,a and sub a
	xor #8
	ld (mute3),a
	call printMute3D
	jp waitForKeyRelease

_muteAll
	ld a,%11000011
	ld (MuteState),a
	ld a,#97
	ld (mute1),a
	ld (mute2),a
	ld (mute3),a
	ld a,#c3				;jp nc = #d2, jp = #c3
	;ld (mute1),a
	ld (muteD),a
	call printMute12
	call printMute3D
	jp waitForKeyRelease
	

kG
	ld a,(CScrType)				;check screen type
	cp 1
	ret nz					;ignore keypress if not on a note ptn screen
	ld b,#10
	jp kHexPtn
kvars
	ret

kminus
	ld a,(CScrType)				;detect current screen type
	or a
	jp nz,waitForKeyRelease			;and exit if not on sequence screen
	
	call findCurrLine			;get pointer to current line into HL

	push hl
	xor a					;clear carry
	ld de,ptn00-5				;point DE to end of ptn sequence - 5
	ex de,hl
	sbc hl,de				;end - 5 - current line = copy length
	ld b,h					;block length now in BC
	ld c,l
	pop hl
	
	ld a,(AlphaFlag)			;check if we're inserting or deleting
	or a
	jr nz,_deleteRow
	
	ld hl,ptn00-6				;source = end of sequence - 4
	ld de,ptn00-2				;dest = end of sequence
	lddr					;copy stuff
	
_reprint	
	call printSeqScr0
	call printCsr
	jp waitForKeyRelease
	
_deleteRow
	ld d,h					;dest now in DE
	ld e,l
	inc hl					;source = dest + 4
	inc hl
	inc hl
	inc hl
	ldir					;copy stuff
	
	ld b,4					;clean up sequence end
	ld hl,ptn00-5
	ld a,#ff
_lp
	ld (hl),a
	inc hl
	djnz _lp		
	
	jp reprintX

	
kplus						;insert next free pattern value at cursor
	ld a,(CScrType)				;detect current screen type
	or a
	jp nz,waitForKeyRelease			;and exit if not on sequence screen
	
	call getSeqOffset			;get source pattern # and save it for later use
	ld (SourcePtn),a
	
	ld a,(CsrPos)
	and %00000110
	cp 6
	jr z,insertFxPtn
	
	xor a
_chklp
	call findNextUnused
	jr z,noFreePtn
	push af
	call isPtnFree
	jr z,freeFound
	pop af
	inc a
	cp #80					;check if all patterns have been searched
	jr nz,_chklp
	
noFreePtn					;handling error if no free ptns found
	ld a,7
	jp errorHand

freeFound
	call getSeqOffset
	pop af
	ld (hl),a
	
	ld a,(AlphaFlag)			;check for Alpha mode
	or a
	jr z,_return
	
	ld a,(SourcePtn)			;if in Alpha mode, copy source pattern to new pattern
	cp #ff					;unless source pattern was empty (#ff)
	;jr z,_return
	jp z,reprintX
	
	ld a,(CsrPos)
	and %00000110
	cp 6
	ld a,(hl)
	jr z,_copyfx
			
	call findPtn				;find the current pattern in memory - pointer is now in DE
	push de					;preserve pointer to it
	
	ld a,(SourcePtn)
	call findPtn				;find source pattern in memory
	ex de,hl				;put pointer to it in HL
	pop de					;retrieve source pointer
				
	ld bc,#10				;copy pattern
_copy
	ldir
	
_return
	jp reprintX


_copyfx
	ld hl,fxptntab
	call findCurrFxPtn
	push de
	
	ld a,(SourcePtn)
	ld hl,fxptntab
	call findCurrFxPtn
	ex de,hl
	pop de
	
	ld bc,#20
	jr _copy


insertFxPtn
	xor a
_chklp
	call findNextUnusedFX
	jr z,noFreePtn
	push af
	call isFxPtnFree
	jr z,freeFound
	pop af
	inc a
	cp #40
	jr nz,_chklp
	

kmult						;copy/del block
	ld a,(CScrType)				;detect current screen type
	or a
	jp nz,waitForKeyRelease			;and exit if not on sequence screen
	
	ld a,(AlphaFlag)			;check Alpha mode
	or a
	jr z,insertBlk
	
_deleteBlock
	ld a,(CPS)				;verify that Block End >= Block Start
	ld b,a	
	ld a,(CPE)	
	sub b
	ld l,a					;save block length (lines) - 1 in L
	ld a,1
	jp c,errorHand				;output error if BS/BE invalid
						
	ld h,0					;calculate block length in memory
	inc hl					;adjust length (always 1 line more than calculated)
	add hl,hl
	add hl,hl
	
	push hl					;block length now in HL
	
	ld bc,ptns
	ld a,(CPS)				;target = (CPS)*4 + ptns
	ld h,0
	ld l,a
	add hl,hl
	add hl,hl
	add hl,bc
	ex de,hl				;target now in DE
	
	ld a,(CPE)				;source = (CPE+1)*4 + ptns
	inc a
	ld h,0
	ld l,a
	add hl,hl
	add hl,hl
	add hl,bc				;source now in HL								
	
	push hl	
	push de
	ex de,hl
	
	ld hl,ptn00-1				;copy length = ptns.end - source +1?
	xor a
	sbc hl,de
	ld b,h
	ld c,l
	
	pop de
	pop hl
	ldir
	
	pop bc					;retrieve block length into bc
	ld hl,ptn00-2
	
_lp2
	ld a,#ff
	ld (hl),a
	dec hl
	dec bc
	ld a,b
	or c
	jr nz,_lp2

	jp reprintX	
	
	

insertBlk	
	ld a,(CPS)				;verify that Block End >= Block Start
	exx
	ld d,a
	exx
	ld b,a	
	ld a,(CPE)
	exx
	ld e,a
	exx
	sub b
	ld b,a					;save block length - 1 in B
	ld a,1
	jp c,errorHand				;and output error if it isn't
	
	call getCurrLineNo
	ld c,a

	inc a					;check that curr line + block length <= #ff
	add a,b
	ld a,8
	jp c,errorHand
	
	ld a,(CPS)
	sub c
	jr nc,_insertBefore			;if BS >= curr line, we're inserting before the selection
	ld a,(CPE)
	sub c
	jr c,_insertAfter			;if curr line > BE, we're inserting after the selection
	ld a,1					;else, curr line is within selection -> output error
	jp errorHand 


_insertBefore
	push bc
	exx
	pop bc
	inc b
	ld a,d
	add a,b
	ld d,a
	ld a,e
	add a,b
	ld e,a
	exx
	

_insertAfter
	ld a,(AlphaFlag)			;check Alpha mode
	or a
	jr nz,_paste				;and skip pre-shifting data if in Alpha mode	
	call calculateCopyParams		;calculate Block start, length, current line #

	ld hl,ptn00-2				;calculate shift block params		
	or a					;clear carry
	sbc hl,bc				;source now in HL
	push hl
	
	call findCurrLine			;current line start now in HL
	
	ex de,hl				;current line start now in DE
	
	pop hl					;retrieve source pointer
	push hl
		
	xor a
	sbc hl,de				;copy block length = source - current line start + 1
	inc hl
	ld b,h
	ld c,l					;copy block length now in BC
	
	pop hl					;retrieve source pointer
	ld de,ptn00-2				;set dest. pointer
	
	lddr					;shift block

_paste
	call calculateCopyParams		;calculate Block start, length, current line #
	push bc

	ld l,a					;calculate copy destination
	ld h,0
	add hl,hl
	add hl,hl				;copy destination - base offset now in HL
	ld bc,ptns
	add hl,bc
	ex de,hl				;copy destination now in DE
	
	add hl,bc				;block start now in HL
	pop bc
	ldir	

	jp reprintX
	

kdiv						;deleting blocks
	ld a,(CScrType)
	or a
	jp nz,waitForKeyRelease
	
	ld a,(AlphaFlag)			;check for Alpha mode
	or a
	jp nz,insertBlk
	jp waitForKeyRelease			;do nothing if Alpha not active


kpot
	ret
	
kclear						;toggle synth (hold) mode
	ld a,(SynthMode)
	cpl
	ld (SynthMode),a
	or a
	ld a,#1d				;disable
	jr z,_set
	
	ld a,#3c				;enable
_set
	ld (timerHold),a
	call printPlayModeIndicator	
	jp waitForKeyRelease
	
	

kenter
	ld a,(PlayerFlag)		;check if player is running
	or a
	jr nz,exitplayer

	ld (exitPlayerSP),sp		;store SP for exiting player
	inc a				;set player flag, A=1
	ld (PlayerFlag),a
	
	call printPlayModeIndicator
	call mplay			;call the music player

exitplayer
exitPlayerSP equ $+1
	ld sp,0				;restore stack
	xor a				;reset player flag
	ld (PlayerFlag),a
	ld a,lp_off			;turn off sound
	out (link),a
	
	call printPlayModeIndicator	
	jp waitForKeyRelease
	;ret				;return to keyhandler
