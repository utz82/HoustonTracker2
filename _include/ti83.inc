;**********************************************************************************
;include file for TI-83/Ion 1.6
;**********************************************************************************

progstart	equ	#9327		;all Ion programs start here

		org progstart
		xor a
		jr nc,begin
		ret

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
lp_msk		equ	#d0d0
lp_smsk		equ	#d1d2		;link port stereo mask

;**********************************************************************************
;saferam addresses

graph_mem	equ	#8E29		;768 bytes	
apd_buf		equ	#8265		;768 bytes
text_mem	equ	#80C9		;128 bytes
text_mem2	equ	#858F		;aka statram, 531 bytes
ops		equ	#8039		;66 bytes

;**********************************************************************************

mem_end		equ	#f600		;last byte of usr RAM 27463 vs 27118 (documented)

;**********************************************************************************
;rom_call macro

macro rom_call,addr
		call addr
endm

macro key_delay
		nop
		nop
endm

;**********************************************************************************
;rom calls

clearlcd	equ	#4755		;aka _clrlcdf

;**********************************************************************************
;shell flags

int_state	equ	drumres		;apd_buf usage flag (disabled by pointing to 0 byte)
