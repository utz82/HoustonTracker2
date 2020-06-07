;*******************************************************************************
;scratch pad memory index and other equates
;*******************************************************************************

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

InputType	equ apd_buf+#14		;1	input type (0=regular, 1=single digit, 2=double digit, 4=word, 6=low nibble)

StateSelect	equ apd_buf+#15		;1	number of the currently selected save slot

AutoInc		equ apd_buf+#16		;1	auto inc mode (1 = off, 0 = on)

RowPlay		equ apd_buf+#17		;1	RowPlay mode (0 = off, #ff = on)

LastKey		equ apd_buf+#18		;1	#a0 if last key set ALPHA mode on, else 0

reptpos		equ apd_buf+#19		;1	number of remaining rows in pattern during playback

SynthMode	equ apd_buf+#20		;1	Synth mode (0 = off, #ff = on).

;ReInit		equ apd_buf+#1a		;1	flag for reinitialization  

PrintBuf	equ ops			;6	print bitmap buffer

FontLUT		equ 256*((HIGH(apd_buf))+1)	;font LUT

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
;Macros
;************************************************************************************


; Set printing cursor at X, Y. DE is destroyed
MACRO setXYat, X, Y
	ld de,X<<8+Y
	call setXY
ENDM

; Prepares DE with a two-byte string. DE is destroyed
MACRO ld_de_TwoChars, firstChar, secondChar
	ld de,firstChar<<8+secondChar
ENDM

; Print a two-byte string. DE is destroyed
MACRO printTwoChars, firstChar, secondChar
	ld_de_TwoChars firstChar, secondChar
	call printDE
ENDM
