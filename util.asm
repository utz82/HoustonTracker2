;*******************************************************************************
;MISC UTILITIES
;*******************************************************************************

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

	setXYat #29, #b8			;print CONF message
	printTwoChars CHAR_C, CHAR_O		;CO
	setXYat #2a, #b8
	printTwoChars CHAR_N, CHAR_F		;NF

_rdkeys
	ld a,KBD_GROUP_ZERO			;read key 0
	out (kbd),a
	key_delay
	in a,(kbd)
	rra
	jp nc,_cancel				;if pressed, cancel user action

	ld a,KBD_GROUP_DOT			;read key .
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
	setXYat #29, #b2			;delete CONF message and rest of msg area
	call clearPrintBuf
	call printBuf
	call printBuf
	setXYat #2a, #b8
	call printBuf
	setXYat #2b, #b2
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
	

findNextUnusedFX				;find the next free fx pattern
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




;*******************************************************************************
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
