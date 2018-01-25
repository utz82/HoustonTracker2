;*******************************************************************************
;HOUSTONTRACKER 2.25.00
;by utz * irrlichtproject.de/houston
;*******************************************************************************

;*******************************************************************************
;OS/shell settings
;*******************************************************************************

TI82 EQU 1					;pass model number to pasmo
TI83 EQU 2					;with --equ MODEL=<model no>
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

	db "HT 2.25", 0				;in-shell title

;*******************************************************************************
;scratch pad index and additional equates
;*******************************************************************************

	include "include/scratchpad.inc"

;*******************************************************************************
;main() init point
;*******************************************************************************

begin
	include "init.asm"			;initialize stuff
	
	call waitForKeyRelease			;make sure no key is pressed before entering keyhandler

	
;The main loop. Yes, it's very simple ;)
;However, there's some inception going on under the hood. Ok, so the main loop calls keyhand. Now, when
;keyhand detects a keypress indicating to start the player, then keyhand will call the player. The
;player then in turn calls keyhand! Now there is keyhand running on top of keyhand. keyhand modifies
;itself in this situation, and disables certain actions like exiting the program. This is to ensure
;that this "inception" is running in a stack-safe manner.
;There are however additional safeguards against stack corruption, see section "shutdown code".

mainlp						
	call keyhand				
	
	jp mainlp

;*******************************************************************************
;keyhandler
;*******************************************************************************

	include "keyhand.asm"
	
;*******************************************************************************
;shutdown code
;*******************************************************************************
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

;*******************************************************************************
;various utility subroutines
;*******************************************************************************

	include "util.asm"

;*******************************************************************************
;graphics driver, print routines
;*******************************************************************************

	include "gfx.asm"

;*******************************************************************************
;load/save routines, compression, savestate management
;*******************************************************************************

	include "mem.asm"
	
;*******************************************************************************
;various tables and includes
;*******************************************************************************

	include "data.asm"
	
;*******************************************************************************
;music driver
;*******************************************************************************

	include "player.asm"

;*******************************************************************************
;work area
;*******************************************************************************
	
musicData				;initialize an empty song on first run		

speed
	db #10				;speed
usrDrum
	dw #0				;usr drum pointer
looprow		
	db 0				;loop point (row#)

ptns					;the pattern matrix
	ds 256*4,#ff			;1024+1 #ff bytes			
	db #ff
	
ptn00					;the note patterns
	ds 16*128			;128*16 #00 bytes
	
fxptn00					;the fx patterns
	ds 32*64			;64*32 #00 bytes

musicEnd equ $				;=savestate LUT

;*******************************************************************************
;savestates
;*******************************************************************************

	db 'XSAVE'			;savestate block header
savestateLUT				;32 byte save state lookup table
	dw savestates			;DEBUG
	dw firstend-1			;DEBUG
	ds 28

savestates				;compressed savestates
	include "teststate.asm"
	
firstend equ $				;debug symbol

;*******************************************************************************
;version signature
;*******************************************************************************

	org mem_end-2
version
	db 1,2				;savestate format version
