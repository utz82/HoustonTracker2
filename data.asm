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

	db CHAR_L,CHAR_P		;LP
	db CHAR_S,CHAR_P		;SP(D)
	db CHAR_B,CHAR_S		;BS
	db CHAR_B,CHAR_E		;BE

notenames
	db CHAR_C,CHAR_DASH		;c
	db CHAR_C,CHAR_SHARP		;c#
	db CHAR_D,CHAR_DASH		;d
	db CHAR_D,CHAR_SHARP		;d#
	db CHAR_E,CHAR_DASH		;e
	db CHAR_F,CHAR_DASH		;f
	db CHAR_F,CHAR_SHARP		;f#
	db CHAR_G,CHAR_DASH		;g
	db CHAR_G,CHAR_SHARP		;g#
	db CHAR_A,CHAR_DASH		;a
	db CHAR_A,CHAR_SHARP		;a#
	db CHAR_B,CHAR_DASH		;b
	



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
