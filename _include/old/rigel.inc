;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё                      ╟╟╟╟╟   ╟╟                  ╟╟                       Ё
;Ё                      ╡шшшш╟  ╡ш                  ╡ш                       Ё
;Ё                      ╡ш  ╟ш                      ╡ш                       Ё
;Ё                      ╡ш  ╟ш  ╟╟   ╟╟╟╟    ╟╟╟╟   ╡ш                       Ё
;Ё                      ╡ш╟╟╟ш  ╡ш   ╡шшш╟   ╡шшш╟  ╡ш                       Ё
;Ё                      ╡шшшш   ╡ш  ╡ш  ╟ш  ╡ш  ╟ш  ╡ш                       Ё
;Ё                      ╡ш  ╟ш  ╡ш  ╡ш  ╟ш  ╡ш╟шш   ╡ш                       Ё
;Ё                      ╡ш  ╟ш  ╡ш  ╡ш╟╟╟ш  ╡шш     ╡ш                       Ё
;Ё                      ╡ш  ╟ш  ╡ш   ╡шшшш   ╡шшшш  ╡ш                       Ё
;Ё                                      ╡ш                                   Ё
;Ё                         Version  ╡ш  ╡ш  0.9 Аeta                         Ё
;Ё                                   ╡шшш                                    Ё
;Ё                                                                           Ё
;Ё                            Copyright (C) 1997                             Ё
;Ё                               Ben Sferrazza                               Ё
;Ё                          bens@nj.semi.harris.com                          Ё
;Ё                                                                           Ё
;Ё                       ╟╠╡ш RIGEL.H Header File ш╡╠╟                       Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды

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

progstart EQU #9296

		org progstart


;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё #DEFINES                                                                  Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
;#DEFINE ROM_CALL(addr)                  call addr
;#DEFINE ROM_CALL_C(addr)                call c, addr
;#DEFINE ROM_CALL_NC(addr)               call nc, addr
;#DEFINE ROM_CALL_Z(addr)                call z, addr
;#DEFINE ROM_CALL_NZ(addr)               call nz, addr
;#DEFINE LIB_CALL_(addr)                 call addr
;#DEFINE LIB_CALL_C(addr)                call c, addr
;#DEFINE LIB_CALL_NC(addr)               call nc, addr
;#DEFINE LIB_CALL_Z(addr)                call z, addr
;#DEFINE LIB_CALL_NZ(addr)               call nz, addr
;#DEFINE CALL_(addr)                     call addr
;#DEFINE CALL_C(addr)                    call c, addr
;#DEFINE CALL_NC(addr)                   call nc, addr
;#DEFINE CALL_Z(addr)                    call z, addr
;#DEFINE CALL_NZ(addr)                   call nz, addr
;#DEFINE JUMP_(addr)                     jp addr
;#DEFINE JUMP_C(addr)                    jp c, addr
;#DEFINE JUMP_NC(addr)                   jp nc, addr
;#DEFINE JUMP_Z(addr)                    jp z, addr
;#DEFINE JUMP_NZ(addr)                   jp nz, addr
;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё Rigel Locations                                                           Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
PROGRAM_ADDR    EQU $8C3C  ; Address of the currently run program
ROM_VER         EQU $8C3E  ; ROM version byte
RIGEL_VER       EQU $8C3F  ; Rigel version byte (BCD), 1st digit is major num,
                         ; second digit is the minor num (e.g. $12 EQU v1.2)
RIGEL_BITS      EQU $8C40  ; bit 0 EQU no longer used - ZShell used it to
                         ;         recalculate the checksum byte
                         ; bit 1 EQU no longer used - ZShell used it to exit
                         ;         to TI-OS upon program completion
                         ; bit 2 EQU reserved
                         ; bit 3 EQU reserved
                         ; bit 4 EQU reserved
                         ; bit 5 EQU reserved
                         ; bit 6 EQU reserved
                         ; bit 7 EQU reserved
PROG_START      EQU $9296  ; The starting address of the currently executed
                         ; string
;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё ROM Routine Locations                                                     Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
;Page 0
;дддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
LD_HL_MHL       EQU $0033  ; HLEQU(HL), AEQUL
CP_HL_DE        EQU $008E  ; CP HL,DE (modifies flags only)
UNPACK_HL       EQU $009A  ; unpacks one digit of HL into %A
STORE_KEY       EQU $01B1  ; store immediate keystroke from %A
GET_KEY         EQU $01BE  ; get immediate keystroke into %A
TX_CHARPUT      EQU $8C41  ; xlated display of %A on screen, normal text style
D_LT_STR        EQU $8C44  ; display length-byte normal text string
M_CHARPUT       EQU $8C47  ; display %A on screen, menu style
D_ZM_STR        EQU $8C4A  ; display zero-terminated string, meny style
D_LM_STR        EQU $8C4D  ; display length-byte string, menu style
GET_T_CUR       EQU $8C50  ; HL EQU absolute address of text cursor
SCROLL_UP       EQU $8C53  ; scroll text screen up
TR_CHARPUT      EQU $8C56  ; raw display of %A on screen, normal text style
CLEARLCD        EQU $8C59  ; clear LCD, but not text or graphics memory
D_HL_DECI       EQU $8C5C  ; disp. HL as 5-byte, right just., blank-pad, decimal
CLEARTEXT       EQU $8C5F  ; clear LCD and text memory (affected by 1, (IY+13))
D_ZT_STR        EQU $8C62  ; display zero-terminated normal text string
BUSY_OFF        EQU $8C65  ; turn off "busy" indicator
BUSY_ON         EQU $8C68  ; turn on "busy" indicator

;Page 4
;дддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд
FIND_PIXEL      EQU $8C6B  ; Find location in RAM for a pixel on the GFX screen

;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё Rigel Routine Locations                                                   Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
INT_INSTALL     EQU $8C6E  ; Install Interrupt with (HL) EQUEQU Address of Routine
INT_REMOVE      EQU $8C71  ; Remove Interrupt
VAR_SEARCH      EQU $8C06  ; Search for a variable in the VAT
;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё Important RAM Locations                                                   Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
KEY_0           EQU $8000  ; translated scancode of last key, but 0 if gotten
KEY_1           EQU $8001  ; translated scancode of key down now
KEY_2           EQU $8002  ; same as 8001, but $FF if more than one key is down
KEY_STAT        EQU $8004  ; bit 2 set EQU key down now
LAST_KEY        EQU $8006  ; last key pressed
CONTRAST        EQU $8007  ; contrast
CURSOR_ROW      EQU $800C  ; text cursor row
CURSOR_COL      EQU $800D  ; text cursor column
CURSOR_LET      EQU $800E  ; letter under text cursor
BUSY_COUNTER    EQU $8080  ; counter for busy indicator
BUSY_BITMAP     EQU $8081  ; bitmap for busy indicator
CURR_INPUT      EQU $80C6  ; -> current home-screen input
BYTES_USED      EQU $80CC  ; # of used user memory (Add to 80C8 to find first 
                         ; byte of free memory)
TEXT_MEM        EQU $80DF  ; text memory
CURSOR_ROW2     EQU $800C  ; text cursor row
CURSOR_COL2     EQU $800D  ; text cursor column
CHECKSUM        EQU $81BE  ; memory checksum from 8BF7 to FA6F
CURSOR_X        EQU $8333  ; x value of cursor
CURSOR_Y        EQU $8334  ; y value of cursor
_IY_TABLE       EQU $8346  ; where IY usually points
GRAPH_MEM       EQU $8641  ; graphics memory
TEXT_MEM2       EQU $8A6B  ; secondary text memory
USER_MEM        EQU $8B1B  ; -> start of user memory
FIXED_POINT     EQU $8B3A  ; fixed-point decimal place (FFh for floating point)
VAT_END         EQU $8BEB  ; -> one byte before end of VAT (backwards)
VAT_START       EQU $FA6F  ; start of VAT
VIDEO_MEM       EQU $FC00  ; video memory
;зддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
;Ё Keypad Scancodes                                                          Ё
;юддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
;                        Function                Character
K_NOKEY         EQU $00    ;No key
K_DOWN          EQU $01    ;Down
K_LEFT          EQU $02    ;Left
K_RIGHT         EQU $03    ;Right
K_UP            EQU $04    ;Up
K_ENTER         EQU $09    ;Enter
K_PLUS          EQU $0A    ;+                      X
K_MINUS         EQU $0B    ;-                      T
K_STAR          EQU $0C    ;*                      O
K_SLASH         EQU $0D    ;/                      J
K_RAISE         EQU $0E    ;^                      E
K_CLEAR         EQU $0F    ;Clear
K_SIGN          EQU $11    ;(-)                    Space
K_3             EQU $12    ;3                      W
K_6             EQU $13    ;6                      S
K_9             EQU $14    ;9                      N
K_RIGHTPAR      EQU $15    ;)                      I
K_TAN           EQU $16    ;Tan                    D
K_CUSTOM        EQU $17    ;Custom
K_DOT           EQU $19    ;.                      Z
K_2             EQU $1A    ;2                      V
K_5             EQU $1B    ;5                      R
K_8             EQU $1C    ;8                      M
K_LEFTPAR       EQU $1D    ;(                      H
K_COS           EQU $1E    ;Cos                    C
K_PRGM          EQU $1F    ;Prgm
K_DEL           EQU $20    ;Del
K_0             EQU $21    ;0                      Y
K_1             EQU $22    ;1                      U
K_4             EQU $23    ;4                      Q
K_7             EQU $24    ;7                      L
K_EE            EQU $25    ;EE                     G
K_SIN           EQU $26    ;Sin                    B
K_STAT          EQU $27    ;Stat
K_XVAR          EQU $28    ;x-Var                  x
K_ON            EQU $29    ;On
K_STO           EQU $2A    ;Sto                    EQU
K_COMMA         EQU $2B    ;,                      P
K_SQUARE        EQU $2C    ;x^2                    K
K_LN            EQU $2D    ;Ln                     F
K_LOG           EQU $2E    ;Log                    A
K_GRAPH         EQU $2F    ;Graph
K_ALPHA         EQU $30    ;Alpha
K_F5            EQU $31    ;F5
K_F4            EQU $32    ;F4
K_F3            EQU $33    ;F3
K_F2            EQU $34    ;F2
K_F1            EQU $35    ;F1
K_SECOND        EQU $36    ;2nd
K_EXIT          EQU $37    ;EXIT
K_MORE          EQU $38    ;MORE
