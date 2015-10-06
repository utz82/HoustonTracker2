;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; CRASH82.INC for CrASH v1.1
;  made by Hideaki Omuro and Jeff Mears
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ



LPON EQU $fc		;link port off
LPOFF EQU $c0		;link port on
LPL EQU $d4		;link port left on
LPR EQU $e8		;link port right on
LPSW EQU $3c		;toggle link port state
LPSWL EQU %00101000	;toggle link port state left
LPSWR EQU %00010100	;toggle link port state right
LMSK EQU $fcfc		;link port mask
LRMASK EQU $d4e8	;link port stereo mask
LPORT EQU 0
KPORT EQU 1

progstart EQU #9104

		org progstart

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; RAM Locations
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;Name         Address Len  Meaning
;ÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄ ÄÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
KEY_0        EQU $8000 ;Byte Scancode of last key, 0 if gotten
KEY_1        EQU $8001 ;Byte Scancode of key down now
KEY_2        EQU $8002 ;Byte Same as $8001, $FF if more than one key is down
KEY_STAT     EQU $8004 ;Byte Bit 2 set EQU key down now
LAST_KEY     EQU $8006 ;Byte Last key pressed
CONTRAST     EQU $8008 ;Byte Contrast ($00 to $1F)
CURSOR_POS   EQU $800C ;Word Text position for word operations
CURSOR_ROW   EQU $800C ;Byte Text cursor row
CURSOR_COL   EQU $800D ;Byte Text cursor column
BUSY_COUNTER EQU $8026 ;Byte Counter for busy indicator
BUSY_BITMAP  EQU $8027 ;Byte Bitmap for busy indicator
op1          EQU $8028 ;  11 OP1
OP2          EQU $8033 ;  11 OP2
OP3          EQU $803E ;  11 OP3
OP4          EQU $8049 ;  11 OP4
OP5          EQU $8054 ;  11 OP5
OP6          EQU $805F ;  11 OP6
TEXT_MEM     EQU $808F ; 128 text memory
GRAF_CURS    EQU $8215 ;Word Cursor position for graphics
CURSOR_X     EQU $8215 ;Byte X value of graphics cursor
CURSOR_Y     EQU $8216 ;Byte Y value of graphics cursor
APD_BUF      EQU $8228 ; 768 APD screen buffer
_IY_TABLE    EQU $8528 ; N/A Where IY usually points
GRAPH_MEM    EQU $88B8 ; 768 Graphics Buffer
TEXT_MEM2    EQU $8BDF ; 128 Secondary text memory
WIN_START    EQU $8C8F ;Byte Number of first row in window
WIN_END      EQU $8C90 ;Byte Number of last row in window
CRASH_VER    EQU $8D73 ;Byte Version number of CrASH
VAT_START    EQU $FE6E ; N/A Start of VAT

; The following will be used in programs using the APD buffer.  If something
; important (ie. interrupt) is going on inside APD_BUF, INT_STATE will not
; be 00. (Look in CrASHprg.TXT for details)
APD_FREE     EQU $8D70 ;Word Public Free Mem of APD_BUF ($000EQUnone, otherEQUusing)
INT_STATE    EQU $8D72 ;Byte Public Stat of Interrupts (00EQUnone, otherEQUusing)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Variable Regions
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; You can allocate memory freely in FREE_A regions.
; You must be careful to not use FREE_B if your program calls CR_KHAND

;Free Memory Regions Len Comments
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
FREE_A1 EQU GRAPH_MEM ;768 Cleared when starting a program
FREE_A2 EQU TEXT_MEM  ;128 Cleared when starting a program
FREE_A3 EQU op1       ; 66 Can be used if only using normal ROM_CALLs
FREE_B  EQU TEXT_MEM2 ;128 Cleared after calling CR_KHAND
FREE_C  EQU APD_BUF   ;768 If INT_STATE is not 0, check for available APD_FREE

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; CrASH call Addresses (use CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; This is a list of useful functions created by the authors of CrASH.  They
; are embedded into CrASH, and are very useful tools.

; Use CALL to call these functions.

EXIT_2_TIOS  EQU $8D88 ; Exits to TI-OS with a clear screen. (can use JP)
RAND         EQU $8D8B ; Returns a random number 0-127 in A.
CP_HL_BC     EQU $8D8E ; Compares HL and BC, like CP_HL_DE
CR_KHAND     EQU $8D91 ; Key_Hand; does not crash if turned off
CR_GRBCopy   EQU $8D94 ; Fast copy to LCD - Read below

; CR_GRBCopy is limited:
;  May not work correctly if LCD_Port is set to go auto right
;  May not work correctly if calc is turboed
;  Only copies from GRAPH_MEM
;  Still testing - please report bugs while using it.

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ROM Addresses (use CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; This is a list of useful functions inside the TI-82's ROM.

; Use CALL to call these functions.

;AddressName  Address Registers Description
;ÄÄÄÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄ ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
LD_HL_MHL    EQU $0033 ;HLA       HL EQU (HL), A EQU L
CP_HL_DE     EQU $0095 ;F         CP  HL, DE
UNPACK_HL    EQU $00A1 ;HLA       HL EQU int(HL/10), A EQU HL MOD 10
DIV_HL_A     EQU $00A3 ;HLA       HL EQU int(HL/A), A EQU HL MOD A
KEY_READ     EQU $01B8 ;AB        Does raw keyboard read with mask A; returns A
STORE_KEY    EQU $01C7 ;?         Store keystroke from A
GET_KEY      EQU $01D4 ;HL?       Get immediate keystroke into A
DISP_DELAY   EQU $07F3 ;          Delay for display controller      (60 clocks)
FIND_PIXEL   EQU $4166 ;?         Find pixel (rom page must be 4)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ROM Offset Addresses (use ROM_CALL)
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; To use functions not included here just make a definition like the ones
; below in your program (not recommended).  If you are using the addresses
; in rom found by another source for rom version 19, remember to subtract $1A
; from that address.  (The ones listed are roms version 16-18)

; Use ROM_CALL to call these functions.

;#DEFINE ROM_CALLaddr) CALL $8D74 \ .DW addr

MACRO ROM_CALL,addr
	call #8d74
	dw addr
ENDM

TR_CHARPUT   EQU $3738 ; Raw display of A on screen, normal text style
TX_CHARPUT   EQU $39D2 ; Xlated display of A on screen, normal text style
D_ZT_STR     EQU $38FA ; Display zero-terminated normal text string
D_LT_STR     EQU $373E ; Display length-byte normal text string
D_HL_DECI    EQU $387C ; Disp HL as 5-char, right just., blank-pad. integer
LAST_LINE    EQU $3774 ; Is it the last line?
NEXT_LINE    EQU $377A ; Goto next line (might scroll)
SCROLL_UP    EQU $3786 ; Scroll text screen up
UP_TEXT      EQU $37F2 ; Update text mem from command shadow

M_CHARPUT    EQU $37CE ; Display %A on screen, menu style
D_ZM_STR     EQU $37D4 ; Display zero-terminated string, menu style
D_LM_STR     EQU $37DA ; Display length indexed string, menu style

CLEARLCD     EQU $389A ; Clear LCD, but not text or graphics memory
CLEARTEXT_W  EQU $37B0 ; Clear LCD and text memory, but only current window (affected by 1,(IY+13))
CLEARTEXT_F  EQU $37A4 ; Clear LCD and text memory (affected by 1,(IY+13))
CLEAR_DISP   EQU $38CA ; Clear display
BACKUP_DISP  EQU $39C6 ; Copy display to APD_BUF
RESTOR_DISP  EQU $38DC ; Copy APD_BUF to display

; In OShell-82, two of these ROM functions are called by the TI-83 name which
; TI gave them.  To make easier for TI-83 programmers, the following two
; definitions are included, although NOT recommended for usage AT ALL.
;                                    ÄÄÄ                       ÄÄÄÄÄÄ
; Also, there are alternate CrASH calls that will do the same thing, but
; enhanced.  (Look above)

DISP_GRAPH   EQU $38AC ; Display contents of GRAPH_MEM on LCD
KEY_HAND     EQU $3924 ; Get key using key handler (2nd, alpha, contrast)

_GRBUFCPY_V  EQU DISP_GRAPH ; Way too slow
_GETKEY      EQU KEY_HAND   ; Crashes when you turn off calc. (not a joke)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Ports
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
P_LINK       EQU $00   ; Link port                                     (TI-85)
P_KEYBOARD   EQU $01   ; Port used to access keys                      (TI-85)
P_ROM        EQU $02   ; Rom page switching and other things
P_STATUS     EQU $03   ; Used to get/set status of varius hardware
P_INT        EQU $04   ; Interrupt speed (don't change!!! the display needs it)
P_LCD_CON    EQU $10   ; Control port for display controller           (TI-83)
P_LCD_DATA   EQU $11   ; Data port for display controller              (TI-83)

;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; Misc
;ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
;#DEFINE TEXT_START RES 1,(IY+0Dh) \ RES 2,(IY+0Dh) \ ROM_CALL(CLEAR_DISP) \ LD HL,0000h \ LD (CURSOR_POS),HL
MACRO TEXT_START
	res 1,(iy+#0d)
	res 2,(iy+#0d)
	ROM_CALL(CLEAR_DISP)
	ld hl,0
	ld (CURSOR_POS),hl
ENDM
MACRO TEXT_END
	set 2,(iy+#0d)
	set 1,(iy+#0d)
ENDM
;#DEFINE TEXT_END SET 2,(IY+0Dh) \ SET 1,(IY+0Dh)

G_NONE   EQU $00
G_DOWN   EQU $01
G_LEFT   EQU $02
G_RIGHT  EQU $03
G_RIGTH  EQU $03 ; Included because author of ASH couldn't spell :)
G_UP     EQU $04
G_ENTER  EQU $09
G_PLUS   EQU $0A
G_MINUS  EQU $0B
G_TIMES  EQU $0C
G_DIV    EQU $0D
G_CARROT EQU $0E
G_CLEAR  EQU $0F
G_NEG    EQU $11
G_3      EQU $12
G_6      EQU $13
G_9      EQU $14
G_PARENR EQU $15
G_TAN    EQU $16
G_VARS   EQU $17
G_PERIOD EQU $19
G_2      EQU $1A
G_5      EQU $1B
G_8      EQU $1C
G_PARENL EQU $1D
G_COS    EQU $1E
G_PRGM   EQU $1F
G_STAT   EQU $20
G_0      EQU $21
G_1      EQU $22
G_4      EQU $23
G_7      EQU $24
G_COMMA  EQU $25
G_SIN    EQU $26
G_MATRIX EQU $27
G_XTO    EQU $28
G_ON     EQU $29
G_STO    EQU $2A
G_LN     EQU $2B
G_LOG    EQU $2C
G_SQR    EQU $2D
G_INVE   EQU $2E
G_MATH   EQU $2F
G_ALPHA  EQU $30
G_GRAPH  EQU $31
G_TRACE  EQU $32
G_ZOOM   EQU $33
G_WINDOW EQU $34
G_YEDIT  EQU $35
G_2ND    EQU $36
G_MODE   EQU $37
G_DEL    EQU $38

K_RIGTH   EQU   1 ; RIGHT     RIGHT
K_RIGHT   EQU   1 ; RIGHT     RIGHT
K_LEFT    EQU   2 ; LEFT      LEFT
K_UP      EQU   3 ; UP        UP
K_DOWN    EQU   4 ; DOWN      DOWN
K_ENTER   EQU   5 ; ENTER     ENTER
K_CLEAR   EQU   6 ; CLEAR     CLEAR
K_DEL     EQU   7 ; DEL       DEL
K_INS     EQU   8 ; DEL       INS
K_RCL     EQU   9 ; STO>      RCL
K_ENTRY   EQU  10 ; ENTER     ENTRY
K_BOL     EQU  11 ; LEFT      BOL
K_EOL     EQU  12 ; RIGHT     EOL
K_PRGM    EQU  45 ; PRGM      PRGM
K_ZOOM    EQU  46 ; ZOOM      ZOOM
K_DRAW    EQU  47 ; PRGM      DRAW
K_PLOT    EQU  48 ; YEQU        STAT PLOT
K_MATH    EQU  49 ; MATH      MATH
K_TEST    EQU  50 ; MATH      TEST
K_VARS    EQU  52 ; VARS      VARS
K_MEM     EQU  53 ; +         MEM
K_MATRIX  EQU  54 ; MATRIX    MATRIX
K_STAT    EQU  55 ; STAT      STAT
K_YVARS  EQU  56 ; VARS      Y-VARS
K_ANGLE   EQU  57 ; MATRIX    ANGLE
K_LIST    EQU  58 ; STAT      LIST
K_CALC    EQU  59 ; TRACE     CALC
K_QUIT    EQU  64 ; MODE      QUIT
K_LINK    EQU  65 ; X,T,theta LINK
K_GRAPH   EQU  69 ; GRAPH     GRAPH
K_MODE    EQU  70 ; MODE      MODE
K_WINDOW  EQU  73 ; WINDOW    WINDOW
K_YEDIT   EQU  74 ; YEQU        YEQU
K_TABLE   EQU  75 ; GRAPH     TABLE
K_TBLSET  EQU  76 ; WINDOW    TblSet
K_TRACE   EQU  93 ; TRACE     TRACE
K_PLUS    EQU 129 ; +         +
K_MINUS   EQU 130 ; -         -
K_TIMES   EQU 131 ; *         *
K_DIV     EQU 132 ; /         /
K_POWER   EQU 133 ; ^         ^
K_LPAREN  EQU 134 ; (         (
K_RPAREN  EQU 135 ; )         )
K_LBRACK  EQU 136 ; *         [
K_RBRACK  EQU 137 ; -         ]
K_STO     EQU 139 ; STO>      STO>
K_COMMA   EQU 140 ; ,         ,
K_NEG     EQU 141 ; (-)       (-)
K_PERIOD  EQU 142 ; .         .
K_0       EQU 143 ; 0         0
K_1       EQU 144 ; 1         1
K_2       EQU 145 ; 2         2
K_3       EQU 146 ; 3         3
K_4       EQU 147 ; 4         4
K_5       EQU 148 ; 5         5
K_6       EQU 149 ; 6         6
K_7       EQU 150 ; 7         7
K_8       EQU 151 ; 8         8
K_9       EQU 152 ; 9         9
K_EE      EQU 153 ; ,         EE
K_SEMI    EQU 154 ; 0         ;
K_A       EQU 155 ; MATH      A
K_B       EQU 156 ; MATRIX    B
K_C       EQU 157 ; PRGM      C
K_D       EQU 158 ; X^-1      D
K_E       EQU 159 ; SIN       E
K_F       EQU 160 ; COS       F
K_G       EQU 161 ; TAN       G
K_H       EQU 162 ; ^         H
K_I       EQU 163 ; X^2       I
K_J       EQU 164 ; ,         J
K_K       EQU 165 ; (         K
K_L       EQU 166 ; )         L
K_M       EQU 167 ; /         M
K_N       EQU 168 ; LOG       N
K_O       EQU 169 ; 7         O
K_P       EQU 170 ; 8         P
K_Q       EQU 171 ; 9         Q
K_R       EQU 172 ; *         R
K_S       EQU 173 ; LN        S
K_T       EQU 174 ; 4         T
K_U       EQU 175 ; 5         U
K_V       EQU 176 ; 6         V
K_W       EQU 177 ; -         W
K_X       EQU 178 ; STO>      X
K_Y       EQU 179 ; 1         Y
K_Z       EQU 180 ; 2         Z
K_XTO     EQU 181 ; X,T,theta X,T,theta
K_PI      EQU 182 ; ^         Pi
K_INVERSE EQU 183 ; X^-1      X^-1
K_SIN     EQU 184 ; SIN       SIN
K_ASIN    EQU 185 ; SIN       SIN^-1
K_COS     EQU 186 ; COS       COS
K_ACOS    EQU 187 ; COS       COS^-1
K_TAN     EQU 188 ; TAN       TAN
K_ATAN    EQU 189 ; TAN       TAN^-1
K_SQR     EQU 190 ; X^2       X^2
K_SQRT    EQU 191 ; X^2       SQRT
K_LN      EQU 192 ; LN        LN
K_EX      EQU 193 ; LN        e^X
K_LOG     EQU 194 ; LOG       LOG
K_TENP    EQU 195 ; LOG       10^X
K_ANS     EQU 198 ; (-)       ANS
K_COLON   EQU 199 ; .         :
K_QUEST   EQU 203 ; (-)       ?
K_QUOTE   EQU 204 ; +         "
K_THETA   EQU 205 ; 3         THETA
K_LBRACE  EQU 237 ; (         {
K_RBRACE  EQU 238 ; )         }
K_ABS     EQU 244 ; X^-1      ABS
K_L1      EQU 245 ; 1         L1
K_L2      EQU 246 ; 2         L2
K_L3      EQU 247 ; 3         L3
K_L4      EQU 248 ; 4         L4
K_L5      EQU 249 ; 5         L5
K_L6      EQU 250 ; 6         L6
K_n       EQU 251 ; 9         n
K_Vn1     EQU 252 ; 8         Vn-1
K_Un1     EQU 253 ; 7         Un-1
