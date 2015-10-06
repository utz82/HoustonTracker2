;an empty song, for initialization

speed
	db #10		;speed
usrDrum
	dw #0		;usr drum pointer
looprow		
	db 0		;loop point (row#)

ptns					;the pattern matrix
	ds 256*4,#ff			;1024+1 #ff bytes
	;db 1,#ff			;shouldn't this be just #ff?
	db #ff
	
ptn00					;the note patterns
	ds 16*128			;128*16 #00 bytes
	
fxptn00					;the fx patterns
	ds 32*64			;64*32 #00 bytes

musicEnd equ $				;=savestate LUT