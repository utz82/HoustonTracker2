;Compressed bitmaps for the font used by HT2.
;
; Five nibbles (2.5 bytes) per character
; Will be unpacked to 5 bytes per character

; Currently 0x00 to 0x1a => 27 different characters
; - Compressed space in RAM = 27*2.5 = 67.5 => 68 bytes = 0x44
; - Uncompressed space in RAM = 27*5 = 135 bytes = 0x87
; Then right-shifted variants of all but the ones that doesn't need it (G, M, STOP, PLAY)
; - Uncompressed space in RAM = (27-4)*5 = 115 bytes = 0x73
; Total needed space in RAM = 135+115=250 bytes, fits in one page.

CHAR_0		equ #00
CHAR_1		equ #01
CHAR_2		equ #02
CHAR_3		equ #03
CHAR_4		equ #04
CHAR_5		equ #05
CHAR_6		equ #06
CHAR_7		equ #07
CHAR_8		equ #08
CHAR_9		equ #09
CHAR_A		equ #0a
CHAR_B		equ #0b
CHAR_C		equ #0c
CHAR_D		equ #0d
CHAR_E		equ #0e
CHAR_F		equ #0f
CHAR_L		equ #10
CHAR_N		equ #11
CHAR_P		equ #12
CHAR_O		equ CHAR_0
CHAR_S		equ #13
CHAR_T		equ #14
CHAR_SHARP	equ #15
CHAR_DASH	equ #16
CHAR_G		equ #17
CHAR_M		equ #18
CHAR_STOP	equ #19
CHAR_PLAY	equ #1a

charNumTotal	equ CHAR_PLAY-CHAR_0+1	; Number of characters, total
charNumShifted	equ CHAR_DASH-CHAR_0+1	; Number of characters to be available as both left and right shifted

IF (charNumTotal+charNumShifted)*5 > 256	; Make sure we don't need more than one page of uncompressed font
.error Uncompressed font will cross page boundary
ENDIF

cmprFontStart
	db %01001010		;0
	db %10101010
	db %01000100		;....|1
	
	db %11000100
	db %01001110
	
	db %11000010		;2
	db %01001000
	db %11101100		;....|3
	
	db %00100100
	db %00101100
	
	db %10001010		;4
	db %11100010
	db %00101100		;....|5
	
	db %10001100
	db %00101100
	
	db %01001000		;6
	db %11001010
	db %01001110		;....|7
	
	db %00100100
	db %01000100
	
	db %01001010		;8
	db %01001010
	db %01000100		;....|9
	
	db %10100110
	db %00100100
	
	db %01101010		;A
	db %11101010
	db %10101100		;....|B
	
	db %10101100
	db %10101100
	
	db %01101000		;C
	db %10001000
	db %01101100		;....|D
	
	db %10101010
	db %10101100
	
	db %11101000		;E
	db %11001000
	db %11101110		;....|F
	
	db %10001100
	db %10001000
	
	db %10001000		;10 - L
	db %10001000
	db %11101100		;....|N
	
	db %10101010
	db %10101010
	
	db %11001010		;12 - P
	db %11001000
	db %10000110		;....|S
	
	db %10000100		
	db %00101100
	
	db %11100100		;14 - T
	db %01000100
	db %01001010		;....|#
	
	db %11101010
	db %11101010
	
	db %00000000		;16 - -
	db %11100000
	db %00000110		;....|G (17)
	
	db %10001010
	db %10101110

	db %10101110		;18 - M
	db %11101010
	db %10100000		;....|STOP (19)
	
	db %11101110
	db %11100000
	
	db %10001100		;PLAY (1a)
	db %11101100
	db %10000000		;...|<unused>
cmprFontSize	equ $-cmprFontStart
