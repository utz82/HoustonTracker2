;**********************************************************************************
;include file for TI-83Plus/TI-84Plus/Ion-compatibles
;**********************************************************************************

progstart	equ	#9d95		;all 8x+ programs start here?
;progstart	equ	#9d93		;all 8x+ programs start here?

		org progstart-2
		db #bb,#6d		;and are prefixed by a token
		xor a
		jr nc,begin
		ret
		
;		jr begin
;		dw 0
;		db #07,#00
;		ds 4
		

;**********************************************************************************
;ports

link		equ	#00		;link port
kbd		equ	#01		;keyboard port
kon		equ	#04		;ON key port???
rom		equ	#07		;ROM page select port
lcd_crt		equ	#10		;lcd control port
lcd_data	equ	#11		;ldc data port

;**********************************************************************************
;link port states

lp_on		equ	#03		;link port off
lp_off		equ	#00		;link port on
lp_l		equ	#01		;link port left on
lp_r		equ	#02		;link port right on
lp_sw		equ	#03		;toggle link port state
lp_swl		equ	#01		;toggle link port state left
lp_swr		equ	#02		;toggle link port state right
;lp_msk		equ	#0303		;link port mask
lp_msk		equ	#0000
lp_smsk		equ	#0102		;link port stereo mask

;**********************************************************************************
;saferam addresses

graph_mem	equ	#9340		;aka plotscreen, 768 bytes	
apd_buf		equ	#9872		;768 bytes
text_mem	equ	#8508		;128 bytes
text_mem2	equ	#8a3a		;aka statram, 531 bytes
ops		equ	#8478		;66 bytes

;**********************************************************************************

;ramend					;24079 (documented, ti83+)
mem_end		equ	#f800		;user memory end (excl. safety margin) In theory, up to ~FB00 would be possible

;**********************************************************************************
;rom_call macro

macro rom_call,addr
		rst #28
		dw addr
endm

macro key_delay
		push hl
		pop hl
endm

;**********************************************************************************
;rom calls

clearlcd	equ	#4540		;aka _clrlcdf

;**********************************************************************************

