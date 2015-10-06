; The Ion Include File by Dan Englender

;----> Safe Memory Areas
; saferam1 = 768 bytes (apdram)
; saferam2 = 531 bytes (statram)
; saferam3 = 128 bytes (textmem)
; saferam4 = 66 bytes (ops)
; saferam5 = 10 bytes (imathptrs)

LPON EQU $03
LPOFF EQU $00
LPL EQU $01
LPR EQU $02
LPSW EQU $03
LPSWL EQU $02
LMSK EQU $0303
LRMASK EQU #0102	;link port stereo mask
LPSWR EQU $01
LPORT EQU 0
KPORT EQU 1

progstart	EQU #9D95

		org progstart-2
		db $BB,$6D
		xor a
		jr nc,begin
		ret

MACRO bcall,addr
	rst #28
	dw addr
ENDM
MACRO bcallz,addr
	jr nz,$+5
	rst #28
	dw addr
ENDM
MACRO bcallnz,addr
	jr z,$+5
	rst #28
	dw addr
ENDM
MACRO bcallc,addr
	jr nc,$+5
	rst #28
	dw addr
ENDM
MACRO bcallnc,addr
	jr c,$+5
	rst #28
	dw addr
ENDM
MACRO bjump,addr
	call #50
	dw addr
ENDM
; .addinstr BCALL *       EF   3 NOP 1
appBackupScreen	EQU #9872	; 768 bytes
_ldhlind	EQU #4009
_htimesl	EQU #4276
_convop1	EQU #4AEF
contrast	EQU #8447
_darkline	EQU #47DD
_darkpoint	EQU #47F2
_archUnarch	EQU #4FD8
_enoughRam	EQU #42FD
usermemoff	EQU #89EC
libstart	EQU #0004
;saferam1	EQU #86EC
saferam1	EQU #9872
saferam2	EQU #858f
saferam3	EQU #8508
saferam4	EQU #8478
saferamp	EQU #9872
saferamp2	EQU #8251
_savedisp	EQU #4C7B
_homeup		EQU #4558
_newline	EQU #452E
_cleargbuf	EQU #4BD0
_indicatorOn	EQU #456D
_indicatorOff	EQU #4570
_copygbuf	EQU #486A
_clrlcdf	EQU #4540
_cleartextshad	EQU #454C
_clrscr		EQU #4543
_clrscrf	EQU #4546
_cline		EQU #4798
_iline		EQU #47E0
_ipoint		EQU #47E3
_ParseInp	EQU #4A9B
_divhlby10	EQU #400F
_cphlde		EQU #400C
_disphl		EQU #4507
_divhlbya	EQU #4012
_hltimes9	EQU #40F9
_vputmap	EQU #455E
_vputs		EQU #4561
_vputsn		EQU #4564
_putmap		EQU #4501
_puts		EQU #450A
_putps		EQU #4510
_putc		EQU #4504
_getkey		EQU #4972
_getk		EQU #4744
_getcsc		EQU #4018
;_RCLY		EQU #4EE0
;_RCLX		EQU #4EE8
_memfree	EQU #42E5
_chksysvar	EQU #42F1
_findsym	EQU #42F4
;_CREATEGDB	EQU #4486
;_CREATEREAL	EQU #4452
;_CREATESTRNG	EQU #4472
_createprog	EQU #4339
_createprotprog	EQU #4E6D
_delvar		EQU #4351
_insertmem	EQU #42F7
_deletemem	EQU #4357
_lcdbusy	EQU #4051
op1		EQU #8478
op2		EQU #8483
op3		EQU #8499
op4		EQU #8499
op5		EQU #84A4
op6		EQU #84AF
imathptr1	EQU #84D3
imathptr2	EQU #84D5
imathptr3	EQU #84D7
imathptr4	EQU #84D9
imathptr5	EQU #84DB
progptr		EQU #9830
cmdshad		EQU #966E
plotsscreen	EQU #9340
symtable	EQU #FE6E
currow		EQU #844B
curcol		EQU #844C
pencol		EQU #86D7
penrow		EQU #86D8
curgy		EQU #8D18
curgx		EQU #8D19
_PushRealO1	EQU #43BA
_PushRealO2	EQU #43B7
_PushRealO3	EQU #43B4
_PushRealO4	EQU #43B1
_PushRealO5	EQU #43AE
_PushRealO6	EQU #43AB
_FPAdd		EQU #4072
_FPDiv		EQU #4099
_FPMult		EQU #4084
_OP1ExOP4	EQU #421C
_OP1Set0	EQU #41BF
_OP3SET2	EQU #41A4
_PopRealO1	EQU #4390
_PopRealO2	EQU #438D
_PopRealO3	EQU #438A
_PopRealO4	EQU #4387
_PopRealO5	EQU #4384
_PopRealO6	EQU #4381
_op5exop6	EQU #420A
_op5set0	EQU #41B3
_cpop1op2	EQU #4111
_op3toop4	EQU #4114
_op1toop4	EQU #4117
_op2toop4	EQU #411A
_op4toop2	EQU #411D
_op3toop2	EQU #4120
_op1toop3	EQU #4123
_op5toop2	EQU #4126
_op5toop6	EQU #4129
_op5toop4	EQU #412C
_op1toop2	EQU #412F
_op6toop2	EQU #4132
_op6toop1	EQU #4135
_op4toop1	EQU #4138
_op5toop1	EQU #413B
_op3toop1	EQU #413E
_op6toop5	EQU #4141
_op4toop5	EQU #4144
_op3toop5	EQU #4147
_op2toop5	EQU #414A
_op2toop6	EQU #414D
_op1toop6	EQU #4150
_op1toop5	EQU #4153
_op2toop1	EQU #4156
_op2toop3	EQU #416E
_op4toop3	EQU #4171
_op5toop3	EQU #4174
_op4toop6	EQU #4177
_OP1Set1	EQU #419B
_OP1Set2	EQU #41A7
_OP1Set3	EQU #41A1
_OP1Set4	EQU #419E
_pushop1	EQU #43C9
_popop1		EQU #437E
_arc_unarc	EQU #4FD8
_sin		EQU #40BD
_zeroop1	EQU #41C5
_zeroop2	EQU #41C8
_zeroop3	EQU #41CB
_setxxop1	EQU #478C
_setxxop2	EQU #478F
_setxxxxop2	EQU #4792
_dispop1a	EQU #4BF7
_mov9toop1	EQU #417A
_delvararc	EQU #4fC6
ptemp		EQU #982E

grbuf		EQU plotsscreen
graph_mem	EQU plotsscreen
gbuf		EQU plotsscreen
sram		EQU saferam1
apdram		EQU saferam1
statram		EQU saferam2
statvars	EQU saferam2
vat		EQU progptr
saferam5	EQU imathptr1
_grbufcpy	EQU #486A
_grbufclr	EQU #4BD0
_clrlcdfull     EQU #4540
_clrscrnfull	EQU #4546
_chkfindsym     EQU #42F1
textshadow	EQU saferam3
savesscreen     EQU saferam1
;-----> Ion Functions
lVectors	EQU cmdshad+80
ionVersion	EQU lVectors
ionRandom	EQU lVectors+3
ionPutSprite	EQU lVectors+6
ionLargeSprite	EQU lVectors+9
ionGetPixel	EQU lVectors+12
ionFastCopy	EQU lVectors+15
ionDetect	EQU lVectors+18
ionDecompress	EQU lVectors+21


trigflags       EQU       0           ; IY OFFSET VALUE
trigdeg         EQU       2
;
plotflags       EQU       2           ; IY OFFSET VALUE
plotloc         EQU       1
plotdisp        EQU       2
;
grfmodeflags    EQU       2           ; IY OFFSET VALUE
grffuncm        EQU       4
grfpolarm       EQU       5
grfparamm       EQU       6
grfrecurm       EQU       7
;
graphflags      EQU       3           ; IY OFFSET VALUE
graphdraw       EQU       0
graphcursor     EQU       2
;
grfdbflags      EQU       4           ; IY OFFSET VALUE
grfdot          EQU       0
grfsimul        EQU       1
grfgrid         EQU       2
grfpolar        EQU       3
grfnocoord      EQU       4
grfnoaxis       EQU       5
grflabel        EQU       6
;
textflags       EQU       5           ; IY OFFSET VALUE
textEraseBelow  EQU       1
textScrolled    EQU       2
textInverse     EQU       3
;
onflags         EQU       9           ; IY OFFSET VALUE
onRunning       EQU       3
onInterrupt     EQU       4
;
statflags       EQU       9           ; IY OFFSET VALUE
statsvalid      EQU       6
;
fmtflags        EQU       10          ; IY OFFSET VALUE
fmtExponent     EQU       0
fmtEng          EQU       1
;
nummode         EQU       10          ; IY OFFSET VALUE
FMTREAL         EQU       5
FMTRECT         EQU       6
FMTPOLAR        EQU       7
;
curflags        EQU       12          ; IY OFFSET VALUE
curAble         EQU       2
curOn           EQU       3
curLock         EQU       4
;
appflags        EQU       13          ; IY OFFSET VALUE
appTextSave     EQU       1
appAutoScroll   EQU       2
;
PLOTFLAG2       EQU       17          ; IY OFFSET VALUE
EXPR_PARAM      EQU       3
EXPR_WRITING    EQU       4
;
indicflags      EQU       18          ; IY OFFSET VALUE
indicRun        EQU       0
indicOnly       EQU       2
;
shiftflags      EQU       18          ; IY OFFSET VALUE
shift2nd        EQU       3
shiftAlpha      EQU       4
shiftALock      EQU       6
;
tblflags        EQU       19          ; IY OFFSET VALUE
AutoFill        EQU       4
AutoCalc        EQU       5
;
sgrflags        EQU       20          ; IY OFFSET VALUE
grfSplit        EQU       0
VertSplit       EQU       1
WRITE_ON_GRAPH  EQU       4
textwrite       EQU       7
;
asm_flag1       EQU       33          ; IY OFFSET VALUE
asm_flag1_0     EQU       0
asm_flag1_1     EQU       1
asm_flag1_2     EQU       2
asm_flag1_3     EQU       3
asm_flag1_4     EQU       4
asm_flag1_5     EQU       5
asm_flag1_6     EQU       6
asm_flag1_7     EQU       7
;
asm_flag2       EQU       34          ; IY OFFSET VALUE
asm_flag2_0     EQU       0
asm_flag2_1     EQU       1
asm_flag2_2     EQU       2
asm_flag2_3     EQU       3
asm_flag2_4     EQU       4
asm_flag2_5     EQU       5
asm_flag2_6     EQU       6
asm_flag2_7     EQU       7
;
asm_flag3       EQU       35          ; IY OFFSET VALUE
asm_flag3_0     EQU       0
asm_flag3_1     EQU       1
asm_flag3_2     EQU       2
asm_flag3_3     EQU       3
asm_flag3_4     EQU       4
asm_flag3_5     EQU       5
asm_flag3_6     EQU       6
asm_flag3_7     EQU       7


