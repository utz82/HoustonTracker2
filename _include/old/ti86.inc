
LPON EQU $fc		;link port off
LPOFF EQU $c0		;link port on
LPL EQU $d4		;link port left on
LPR EQU $e8		;link port right on
LPSW EQU $3c		;toggle link port state
LPSWL EQU %00101000	;toggle link port state left
LPSWR EQU %00010100	;toggle link port state right
LMSK EQU $fcfc		;link port mask
LRMASK EQU $d4e8	;link port stereo mask
LPORT EQU 7
KPORT EQU 1

progstart EQU #D748

		org progstart


;====================================================================
;These are common zshell and usgard calls that are known on the TI-86
;If possible, use the call that is commented next to the one you want
;====================================================================
MUL_HL EQU  $4547

CP_HL_DE EQU  $403C
LD_HL_MHL EQU  $4010
GET_KEY EQU  $5371
UNPACK_HL EQU  $4044

D_HL_DECI EQU  $4a33

BUSY_OFF EQU  $4ab1			;_runindoff
BUSY_ON EQU  $4aad				;_runindicon
D_ZT_STR EQU  $4a37			;_puts
D_LT_STR EQU  $4a3b			;_putps
TX_CHARPUT EQU  $4a2b			;_putc
D_ZM_STR EQU  $4aa5			;_vputs
D_LM_STR EQU  $4aa9			;_vputsn
M_CHARPUT EQU  $4aa1			;_vputmap
CLEARLCD EQU  $4a7e			;_clrLCD


;========================================================
;These are memory addresses common for zshell programming
;If possible, use the one on the right
;========================================================

CONTRAST EQU  $c008			
CURSOR_ROW EQU  $c00f			;_curRow
CURSOR_COL EQU  $c010			;_curCol
BUSY_COUNTER EQU  $c087 
BUSY_BITMAP	EQU  $c088
TEXT_MEM EQU  $c0f9			;_textShadow
CURSOR_X EQU  $c37c			;_penCol
CURSOR_Y EQU  $c37d			;_penRow
GRAPH_MEM EQU  $c9fa			;_plotSScreen
TEXT_MEM2 EQU  $cfab			;_cmdShadow
VAT_END EQU  $d298
VAT_START EQU  $8000
VIDEO_MEM EQU  $fc00

;==================================================================
;all the keys are used with <call GET_KEY>, not TI's <call _getkey>
;==================================================================
K_NOKEY       EQU $00    ;No key
K_DOWN        EQU $01    ;Down
K_LEFT        EQU $02    ;Left
K_RIGHT       EQU $03    ;Right
K_UP          EQU $04    ;Up
K_ENTER       EQU $09    ;Enter
K_PLUS        EQU $0A    ;+                      X
K_MINUS       EQU $0B    ;-                      T
K_STAR        EQU $0C    ;*                      O
K_SLASH       EQU $0D    ;/                      J
K_RAISE       EQU $0E    ;^                      E
K_CLEAR       EQU $0F    ;Clear
K_SIGN        EQU $11    ;(-)                    Space
K_3           EQU $12    ;3                      W
K_6           EQU $13    ;6                      S
K_9           EQU $14    ;9                      N
K_RIGHTPAR    EQU $15    ;)                      I
K_TAN         EQU $16    ;Tan                    D
K_CUSTOM      EQU $17    ;Custom
K_DOT         EQU $19    ;.                      Z
K_2           EQU $1A    ;2                      V
K_5           EQU $1B    ;5                      R
K_8           EQU $1C    ;8                      M
K_LEFTPAR     EQU $1D    ;(                      H
K_COS         EQU $1E    ;Cos                    C
K_PRGM        EQU $1F    ;Prgm
K_DEL         EQU $20    ;Del
K_0           EQU $21    ;0                      Y
K_1           EQU $22    ;1                      U
K_4           EQU $23    ;4                      Q
K_7           EQU $24    ;7                      L
K_EE          EQU $25    ;EE                     G
K_SIN         EQU $26    ;Sin                    B
K_TABLE       EQU $27    ;Table		  ;Used to be Stat on the TI-85, now K_TABLE
K_XVAR        EQU $28    ;x-Var                  x
K_ON          EQU $29    ;On
K_STO         EQU $2A    ;Sto                    EQU 
K_COMMA       EQU $2B    ;,                      P
K_SQUARE      EQU $2C    ;x^2                    K
K_LN          EQU $2D    ;Ln                     F
K_LOG         EQU $2E    ;Log                    A
K_GRAPH       EQU $2F    ;Graph
K_ALPHA       EQU $30    ;Alpha
K_F5          EQU $31    ;F5
K_F4          EQU $32    ;F4
K_F3          EQU $33    ;F3
K_F2          EQU $34    ;F2
K_F1          EQU $35    ;F1
K_SECOND      EQU $36    ;2nd
K_EXIT        EQU $37    ;EXIT
K_MORE        EQU $38    ;MORE
