;**********************************************************************************
;include file for TI-82/CrASH 1.1
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

lp_on		equ	#d3		;link port off
lp_off		equ	#d0		;link port on
lp_l		equ	#d1		;link port left on
lp_r		equ	#d2		;link port right on
lp_sw		equ	#03		;toggle link port state
lp_swl		equ	#01		;toggle link port state left
lp_swr		equ	#02		;toggle link port state right
;lp_msk		equ	#d3d3		;link port mask
lp_msk		equ	#d0d0		;link port mask
lp_smsk		equ	#d1d2		;link port stereo mask

;**********************************************************************************
;saferam addresses

graph_mem	equ	#88b8		;768 bytes	
apd_buf		equ	#8228		;768 bytes
text_mem	equ	#808f		;128 bytes
text_mem2	equ	#8bdf		;128 bytes
ops		equ	#8028		;66 bytes

mem_end		equ	#f600		;user memory end (excl. safety margin)

;**********************************************************************************
;rom_call macro

macro rom_call,addr
		;call #8d74
		;dw addr
		call addr
endm

macro key_delay
		push hl
		pop hl
endm

;**********************************************************************************
;rom calls

clearlcd	equ	#39cd		;clear LCD, but not text or graphics memory
cleartext_f	equ	#37A4		;clear text_mem

;**********************************************************************************
;shell flags

int_state	equ	#8d72		;apd_buf usage flag

