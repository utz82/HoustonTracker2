;memory related subroutines
;************************************************************************************
stateSelect				;prompt user to select a savestate (slot) number
	ld a,(CScrType)
	cp 2
	
	call z,delCsr2
	jr _skip
	call delCsr
_skip
	jp inputSlotNr

;************************************************************************************
delSlot					;delete a save slot

	ld_de_TwoChars CHAR_D, CHAR_S	;print "DS" message
	call printMsg	
	call stateSelect
	call confirmAction
	ret c
	
delSlotNoConfirm			;delete slot without asking for confirmation
	ld a,(StateSelect)		;read save slot number
	add a,a				;calculate offset in savestate LUT
	add a,a
	ld e,a
	ld d,0
	ld hl,savestateLUT		;calculate pointer to LUT entry
	add hl,de
	
	ld e,(hl)			;load lo byte of delslot.start
	inc hl
	ld a,(hl)			;check if savestate is empty
	or a	
	ret z				;and exit if that's the case
	
	ld d,a				;delslot.start now in DE
	
	dec hl				;preserve LUT pointer on stack
	push hl
	inc hl
	
	inc hl
	ld c,(hl)
	inc hl
	ld b,(hl)
	inc bc				;delslot.end+1 now in BC
	push bc
	
	xor a
	ld hl,mem_end-3
	sbc hl,bc
	
	ld b,h
	ld c,l				;remaining block length now in BC
	pop hl				;delslot.end+1 in in HL
	
	push de
	push hl
	
	ldir				;copy remaining block to new location

delUpdateLUT	
	pop hl
	dec hl				;delslot.end
	pop de				;delslot.start
	
	xor a
	sbc hl,de			;delslot.length now in HL
	
	pop bc				;retrieve LUT pointer
	ld (bc),a			;clear LUT entry
	inc bc
	ld (bc),a
	inc bc
	ld (bc),a
	inc bc
	ld (bc),a

	ld (_oldSP),sp			;preserve stack pointer	
	ld sp,hl			;delslot.length now in SP
	ld hl,savestateLUT
	ld b,d				;delslot.start now in BC
	ld c,e
	exx
	ld b,#10			;updating 16 word-length entries (8 long)

_lp
	exx	
	ld e,(hl)			;read entry from savestate LUT
	inc hl
	ld d,(hl)
	
	ex de,hl	
	xor a
	sbc hl,bc			;check if entry < delslot.start (or entry == 0)	
	ex de,hl
	
	jr c,_noupdate			;if so, move on to the next entry
		
	ex de,hl
	add hl,bc			;restore slot val	
	sbc hl,sp			;subtract delslot.length from entry
	ex de,hl

	dec de	
	ld (hl),d			;store updated slot val in LUT
	dec hl
	ld (hl),e
	inc hl
	
_noupdate
	inc hl
	exx
	djnz _lp

_oldSP equ $+1				;restore stack pointer
	ld sp,0
	
	;jp waitForKeyRelease
	ret

;************************************************************************************
zap					;delete song currently loaded in work area
	ld hl,#10			;clear speed, usr drum, lp
	ld (musicData),hl
	ld l,0
	ld (musicData+2),hl
	
	ld hl,musicData+4		;clearing sequence (writing 1025 #ff bytes)
	ld de,musicData+5
	;ld bc,1024
	ld bc,1025			;NEW
	ld a,#ff
	ld (hl),a
	ldir
	
	xor a				;clearing pattern area (writing 4096 #00 bytes)
	ld (hl),a
	ld bc,4095
	ldir
	
	jp resetFX0

;************************************************************************************
addressCompare				;compare two 16-bit values
					;IN: val 1 in DE, val 2 in BC | OUT: higher val in BC

	ex af,af'
	inc a				;signal to save code that a savestate has been found
	ex af,af'

	;dec hl
	ld e,(hl)
	inc de
	push hl
	
	ld h,b
	ld l,c
	
	xor a				;clear carry (should in theory be cleared by previous or d)	
	sbc hl,de
	jr nc,_return			;if BC > DE, all is good
	
	ld b,d				;else, DE -> BC
	ld c,e
	
_return	
	pop hl
	;inc hl				;pointer adjustment
	ret

;************************************************************************************
save					;save a compressed backup savestate of the current song

	call delSlotNoConfirm		;if savestate exists in the current slot, delete it
	
_findMemLoc				;iterate through savestate LUT to find the next free memory location
	ld hl,savestateLUT+31
	ld bc,0
	
	xor a
	ex af,af'
	;xor a
	exx
	ld b,8
	

_findlp
	xor a
	exx
	ld d,(hl)			;read hi byte of slot.end
	or d				;if it's non-zero, this is an existing savestate
	dec hl

	call nz,addressCompare
			
	dec hl				;else, decrement pointer
	dec hl
	dec hl
	exx
	djnz _findlp			;and loop

	exx	
	ld de,savestates		;if no existing save states are found, set MemLoc to start of savestate area
	
	ex af,af'
	or a				;A' holds # of savestates found TODO BUG: A=0 even though it shouldn't be, likewise BC=0
	jr z,_proceed
	
	ld d,b
	ld e,c

_proceed
	ld a,(StateSelect)		;read selected state #
	add a,a
	add a,a
	ld c,a
	ld b,0
	ld hl,savestateLUT
	add hl,bc			;point to it in LUT
	ld (hl),e			;and write start address of the slot to be saved to LUT
	inc hl
	ld (hl),d
	
	inc hl
	ld (LUTpointer),hl		;preserve pointer address for writing slot.end value later on
	
;******
	;ld de,savestates		;DEBUG
					;DE holds start address (target)
	ld hl,musicData
	ld bc,#04ff			;b=counter, c=bogus high value to obfuscate decrement by ldi
	
_savevars
	ldi				;save global vars
	call chkMemEnd			;test for out of mem error
	djnz _savevars
	
_saveseq				;save song sequence
	ld a,(hl)
	ldi				;save 1 byte
	call chkMemEnd			;test for out of mem error
	inc a				;check if last saved byte was #ff
	jr nz,_saveseq			;save next byte if it wasn't

_saveptns
	ex de,hl			;now HL = store pointer
	ld de,ptn00			;DE = read pointer
	
_saveptnlp
	call chkLastPtn			;check if all ptns have been saved
	jr nz,_skip1	
	ld a,#ff			;if yes, write an #ff byte
	ld (hl),a
	inc hl				;update pointers
	jr saveFX			;and continue with fx ptns
	
_skip1
	ld a,#df			;assume that the following n patterns are empty
	ld (hl),a			;write #df byte
	ld c,#1f			;C = # of patterns to check, checking the following #20 patterns
	
_isPtnEmpty				;check if current pattern is empty
	ld b,#10			;check #10 rows
	push de
_lp
	ld a,(de)			;check if byte == 0
	or a
	jr nz,_isRowEmpty		;if it isn't, continue with checking for empty rows
	inc de				;else, inc pointer
	djnz _lp			;and check next row
	
	pop de				;retrieve load pointer
	inc (hl)			;increment "empty ptns" counter - DOESN'T THIS HAVE TO BE inc (hl)??? (was inc hl)
					;ATTN: SURE? 

	push hl				;increment load pointer by #10
	ld hl,#10
	add hl,de
	ex de,hl
	pop hl
	
	dec c				;decrement "# of ptns to check" counter
	jr nz,_isPtnEmpty		;and check next ptn if done with all #20 ptns
	
	inc hl				;else, increment store pointer
	ex de,hl
	call chkMemEnd			;check for mem_end
	ex de,hl
	jr _saveptnlp			;and move on to the next ptn
	

_isRowEmpty
	pop de				;retrieve load pointer
	
	ld a,(hl)			;NEW: check if e0 byte was previously written
	cp #e0				;NEW:
	jr c,_skipxx			;NEW:
	inc hl				;NEW: and inc save pointer if necessary
	
_skipxx	
	ld a,#cf			;assume next row is empty
	ld (hl),a
	ld b,#10			;check up to #10 rows
_lp2
	ld a,(de)			;check if byte == 0
	or a
	jr nz,_rNotEmpty		;if byte != 0, row isn't empty
	
	inc (hl)			;else, increment "empty rows" counter
	inc de				;increment load pointer
	
_exitlp
	djnz _lp2			;and keep checking for empty rows if not all have been checked yet

	ld a,#cf			;NEW 20-09: unless end of pattern reached with non-empty row, inc write pointer
	cp (hl)
	jr z,_skipxxx
	inc hl				
_skipxxx
	jr _saveptnlp			;else, move on to the next ptn
	
_rNotEmpty
	ld a,(hl)			;if row wasn't empty
	cp #d0				;check if an "empty rows" or "empty ptns" counter was previously written
	jr c,_skipx
	inc hl				;increment store pointer if yes
_skipx
	ex de,hl
	ldi				;copy 1 byte
	call chkMemEnd			;check for mem_end
	inc bc				;adjust counter
	ex de,hl
	ld a,#cf			;assume next row is empty
	ld (hl),a
	jr _exitlp			;and move on to the next row

	
saveFX					;compress and save the fx patterns
	ld de,fxptn00
	ex de,hl			;now DE = store pointer and HL = read pointer again

	push hl				;check if all fx ptns are empty.
	ld bc,#800			;check following 2048 bytes
	
_clp
	ld a,(hl)
	or a
	jr nz,_cont
	inc hl
	dec bc
	ld a,b
	or c
	jr nz,_clp
	
	pop hl				;If all fx ptns were empty, write #ff byte and exit.
	cpl
	ld (de),a
	jp _success+1

_cont
	pop hl	
	ex af,af'
	ld a,#ff			;AF' = pattern counter
	ex af,af'
	
_sfxlp
	ex af,af'
	inc a				;inc pattern counter
	cp #40				;SHOULD BE cp #40? if it's #40 or bit 7 is set, we're done saving
	;ret nc
	jp nc,_success
	ex af,af'
	
_lookAhead				;check if the next ptn is empty (next 32B == 0)
	ld b,#20
	push hl				;preserve read pointer
_lAlp	
	ld a,(hl)			;check if bytes == 0
	or a
	inc hl
	jr nz,_fxNotEmpty		;if byte != 0, pattern isn't empty
	djnz _lAlp
	
	pop hl				;if pattern was empty, retrieve read pointer
	ld bc,#20			;point to next pattern (add 32)
	add hl,bc
	jr _sfxlp			;repeat from start
	
_fxNotEmpty
	pop hl
	push hl
	ld bc,#20
	add hl,bc
_fxNElp
	ld a,(hl)			;check if remaining fx ptns (except the current one) are empty
	or a
	inc hl
	jr nz,_writeFxPtn		;if byte != 0 found, copy ptn normally
	
	ld a,h				;check if end of work area reached
	cp HIGH(musicEnd)
	jr nz,_fxNElp
	ld a,l
	cp LOW(musicEnd)
	jr nz,_fxNElp			;if not, keep parsing fx ptns
	
	ex af,af'			;if all remaining fx ptns were empty
	add a,#80			;set bit 7 of the ptn counter
	ex af,af'

_writeFxPtn
	ex af,af'
	ld (de),a			;write ptn # at store pointer
	ex af,af'
	pop hl				;retrieve load pointer
	inc de
	;ld bc,#20			;copying 32 bytes
	ld b,#20
_writelp
	ldi				;transfer a byte
	call chkMemEnd			;check for mem_end
	;ldi
	;jr nz,_writelp
	djnz _writelp
	jr _sfxlp			;and on to the next pattern

_success
	dec de
	;ld a,(StateSelect)
LUTpointer equ $+2	
	ld (savestateLUT+2),de		;write end address to savestate LUT
	
	ld a,(StateSelect)
	call printSaveSlotIndicator
	
	xor a
	jp errorHand	
;************************************************************************************
chkLastPtn				;check if all used note patterns have been saved
					;OUT: Z if all used ptns have been saved, else NZ
	push de
_lp
	ld a,d				;check if end of note ptn area reached
	cp HIGH(fxptn00)
	jr nz,_skip
	ld a,e
	cp LOW(fxptn00)
	jr z,_skip2
_skip	
	ld a,(de)			;check if byte is empty
	or a
	inc de
	jr z,_lp			;if it is, check next byte

_skip2					;else return
	pop de
	ret
	;jp waitForKeyRelease		;not needed since kDot is ignored when returning
;************************************************************************************

chkMemEnd				;check if end of usr memory reached
					;IN: nothing | OUT: jumps to errorHand if mem_end reached
	or a				;clear carry
	push hl
	ld hl,mem_end-4
	;ld hl,savestates+2		;DEBUG
	sbc hl,de			;subtract current mempos from mem_end-3
	pop hl
	ret nc

saveError				;handling out of memory errors
	pop hl				;pop useless return address
	
	ld hl,savestateLUT		;delete savestateLUT entry
	ld a,(StateSelect)
	add a,a
	add a,a
	ld e,a
	ld d,0
	add hl,de
	xor a
	ld (hl),a
	inc hl
	ld (hl),a
	
	ld a,2				;set error code
	jp errorHand0

;************************************************************************************
load					;load a song from a backup savestate.
					;IN: # of savestate to load in (StateSelect) | OUT: nothing, AF,BC,DE,HL destroyed.
	call zap			;clear work area					

	ld a,(version)			;check savestate format version
	;or a
	dec a
	jr z,_ldstart
	ld a,6				;if version != 0, abort loading and generate error
	jp errorHand0
	
_ldstart
; 	ld hl,ptns			;initialize sequence with #ff bytes
; 	ld de,ptns+1			;this is all unnecessary since we zap before
; 	ld bc,1025
; 	ld a,#ff
; 	ld (hl),a
; 	ldir
; 	
; 	xor a				;initialize rest of work area with #00
; 	ld (hl),a
; 	ld bc,16*256
; 	ldir

	ld hl,savestateLUT		;set pointer to savestate LUT
	ld a,(StateSelect)		;calculate offset
	add a,a
	add a,a				;NEW: each LUT entry is now 4 bytes
	ld e,a
	ld d,0
	add hl,de			;add it to LUT pointer
	ld e,(hl)			;get address of savestate to load in DE
	inc hl
	ld a,(hl)
	or a				;trap empty savestates
	ld a,5
	jp z,errorHand0			;abort and ouput error if empty savestate encountered
	ld d,(hl)
	ex de,hl			;move it to HL
	ld de,musicData			;set destination address

_ldvars					;load global song vars
	ldi				;speed
	ldi				;usr drum pointer
	ldi
	ldi				;loop point

_ldseq					;load sequence
	ld a,(hl)			;check for seq. end marker (#ff)
	cp #ff
	jr z,_ldptns			;continue with loading patterns if end marker found
	ldi				;else, move byte from savestate to work area
	jr _ldseq
	
_ldptns	
	inc hl				;adjust pointer to savestate
	ld de,ptn00			;adjust pointer to work area

_ldptnlp	
	ld a,(hl)			;get a byte from savestate
	cp #ff				;check for ptn area end marker (#ff)
	jr z,_ldfxptns			;if end marker is found, continue with loading fx patterns
	
	push hl				;preserve pointer to savestate
	ld h,0				;init offset value
	
	cp #e0				;check if byte >= #e0 ("empty patterns")
	jr c,_skip1
	sub #df				;offset = ([hl] - #df)*16
	ld l,a
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	jr _skip3

_skip1	
	cp #d0				;check if byte >= #d0 ("empty rows")
	jr c,_noCP			;if it isn't, we have normal uncompressed data
	sub #cf				;else, offset = [hl] - #cf
	ld l,a				

_skip3		
	add hl,de			;add offset to work area pointer
	ex de,hl
	pop hl				;restore savestate pointer
	inc hl				;increment savestate pointer
	jr _ldptnlp			;read next byte

_noCP
	pop hl				;restore savestate pointer
	ldi				;transfer byte to work area and increment pointers
	jr _ldptnlp			;read next byte
	
	
_ldfxptns
ldfxpnts
	inc hl				;adjust pointer to savestate
	;ld de,fxptn00			;adjust pointer to work area
	
_ldfxptnlp				;TODO: optimize ptn address finding by using fxptntab lookup
	ld de,fxptn00			;adjust pointer to work area	
	ld a,(hl)			;load ptn #
	
	cp #ff				;if ptn# == #ff, there are no fx patterns to load
	ret z
	
	push af
	;and #7f				
	and #3f				;mask out bit 6,7
	
	inc hl
	push hl

	ld h,0
	
	add a,a
	add a,a
	
	ld l,a				;patnum is max #3f, so can add a,a twice before loading into hl
	
	;add hl,hl			;offset = ptn# * 32
	;add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	
	add hl,de			;add offset to work area pointer
	ex de,hl
	
	pop hl
	
	ld bc,32			;copy 32 bytes
	ldir
	
	pop af
	cp #3f				;check if bit 7 of ptn# was set or ptn# was >#3f
	jr c,_ldfxptnlp			;TEST DEBUG
	ret				;else we're done.
;************************************************************************************	