;**********************************************************************************
;include file for TI-82/CrASH 1.6
;**********************************************************************************

progstart	equ	#9104		;all CrASH programs start here

		org progstart

;**********************************************************************************
;ports

link		equ	#00		;link port
kbd		equ	#01		;keyboard port
rom		equ	#02		;ROM page select port
kon		equ	#03		;ON key port
lcd_crt		equ	#10		;lcd control port
lcd_data	equ	#11		;ldc data port

;**********************************************************************************
;link port states

lp_on		equ	#fc		;link port off
lp_off		equ	#c0		;link port on
lp_l		equ	#d4		;link port left on
lp_r		equ	#e8		;link port right on
lp_sw		equ	#3c		;toggle link port state
lp_swl		equ	#28		;toggle link port state left
lp_swr		equ	#14		;toggle link port state right
;lp_msk		equ	#fcfc		;link port mask
lp_msk		equ	#c0c0
lp_smsk		equ	#d4e8		;link port stereo mask

;**********************************************************************************
;saferam addresses

graph_mem	equ	#88b8		;768 bytes	
apd_buf		equ	#8228		;768 bytes
text_mem	equ	#808f		;128 bytes
text_mem2	equ	#8bdf		;128 bytes
ops		equ	#8028		;66 bytes

mem_end		equ	#fcb3		;user memory end (excl. safety margin)

;**********************************************************************************
;rom_call macro

macro rom_call,addr
		call #8d74
		dw addr
endm

macro key_delay
		nop
		nop
endm


;**********************************************************************************
;rom calls

clearlcd	equ	#389a		;clear LCD, but not text or graphics memory
cleartext_f	equ	#37A4		;clear text_mem

;**********************************************************************************
;shell flags

int_state	equ	#8d72		;apd_buf usage flag

