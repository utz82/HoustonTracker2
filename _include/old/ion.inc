; The Ion Include File by Dan Englender

;----> Safe Memory Areas
; saferam1 = 768 bytes (apdram)
; saferam2 = 531 bytes (statram)
; saferam3 = 128 bytes (textmem)
; saferam4 = 66 bytes (ops)
; saferam5 = 10 bytes (imathptrs)

LPON EQU $d3
LPOFF EQU $d0
LPL EQU $d1
LPR EQU $d2
LPSW EQU $03
LPSWL EQU $02
LMSK EQU $d3d3
LRMASK EQU #d1d2	;link port stereo mask
LPSWR EQU $01
LPORT EQU 0
KPORT EQU 1

MACRO bcall,addr
	call addr
ENDM
MACRO bcallz,addr
	call z,addr
ENDM
MACRO bcallnz,addr
	call nz,addr
ENDM
MACRO bcallc,addr
	call c,addr
ENDM
MACRO bcallnc,addr
	call nc,addr
ENDM
MACRO bjump,addr
	jp addr
ENDM

progstart	EQU #9327

		org progstart
		xor a
		jr nc,begin
		ret

; .addinstr BCALL *       CD   3 NOP 1
_htimesl	EQU #4382
usermemoff	EQU #8565
libstart	EQU #0002
saferam1	EQU #8265
saferam2	EQU #858F
saferam3	EQU #80C9
saferam4	EQU #8039
_savedisp	EQU #4859
_zeroop1	EQU #428E
_getcsc		EQU #4014
_homeup		EQU #4775
_newline	EQU #473D
_darkline	EQU #4AE0
_indicatorOff	EQU #4795
_indicatorOn	EQU #4791
_cleargbuf	EQU #515B
_copygbuf	EQU #4B9C
_clrlcdf	EQU #4755
_cleartextshad	EQU #4765
_clrscr		EQU #4759
_clrscrf	EQU #475D
_cline		EQU #4A84
_iline		EQU #4AE4
_ipoint		EQU #4AE8
_ldhlind	EQU #4000
_divhlby10	EQU #4008
_cphlde		EQU #4004
_disphl		EQU #4709
_divhlbya	EQU #400C
_hltimes9	EQU #4146
_vputmap	EQU #477D
_vputs		EQU #4781
_vputsn		EQU #4785
_putmap		EQU #4701
_puts		EQU #470D
_putps		EQU #4715
_parseinp       EQU #4E8C
_putc		EQU #4705
_getkey		EQU #4CFE
_getk		EQU #4A18
_rcly		EQU #4EE0
_rclx		EQU #4EE8
_memfree	EQU #441A
_chksysvar	EQU #442A
_findsym	EQU #442E
_creategdb	EQU #4486
_createprog	EQU #448A
_createreal	EQU #4452
_createstring	EQU #4472
_delvar		EQU #44AA
_insertmem	EQU #4432
_deletemem	EQU #44B2
_lcdbusy	EQU #4066
ptemp		EQU #9317
op1		EQU #8039
op2		EQU #8044
op3		EQU #804F
op4		EQU #805A
op5		EQU #8065
op6		EQU #8070
imathptr1	EQU #8094
imathptr2	EQU #8096
imathptr3	EQU #8098
imathptr4	EQU #809A
imathptr5	EQU #809C
progptr		EQU #9319
cmdshad		EQU #9157
plotsscreen	EQU #8E29
symtable	EQU #FE6E
currow		EQU #800C
curcol		EQU #800D
pencol		EQU #8252
penrow		EQU #8253
curgy		EQU #886D
curgx		EQU #886E
_op1toop2	EQU #418E
_op1toop3	EQU #417E
_op1toop4	EQU #416E
_op1toop5	EQU #41BE
_op1toop6	EQU #41BA
_op2toop1	EQU #41C2
_op2toop3	EQU #41F6
_op2toop4	EQU #4172
_op2toop5	EQU #41B2
_op2toop6	EQU #41B6
_op3toop1	EQU #41A2
_op3toop2	EQU #417A
_op3toop4	EQU #416A
_op3toop5	EQU #41AE
_op4toop1	EQU #419A
_op4toop2	EQU #4176
_op4toop3	EQU #41FA
_op4toop5	EQU #41AA
_op4toop6	EQU #4202
_op5toop1	EQU #419E
_op5toop2	EQU #4182
_op5toop3	EQU #41FE
_op5toop4	EQU #418A
_op5toop6	EQU #4186
_op6toop1	EQU #4196
_op6toop2	EQU #4192
_op6toop5	EQU #41A6
_op1exop2	EQU #4306
_op1exop3	EQU #42FE
_op1exop4	EQU #4302
_op1exop5	EQU #42EE
_op1exop6	EQU #42F2
_op2exop4	EQU #42F6
_op2exop5	EQU #42FA
_op2exop6	EQU #42E6
_op5exop6	EQU #42EA
_PUSHREALO1     EQU #4536
_PUSHREALO4     EQU #452A
_POPREALO2      EQU #44FA
_POPREALO5      EQU #44EE
_PUSHREALO2     EQU #4532
_PUSHREALO5     EQU #4526
_POPREALO3      EQU #44F6
_POPREALO6      EQU #44EA
_PUSHREALO3     EQU #452E
_PUSHREALO6     EQU #4522
_POPREALO1      EQU #44FE
_POPREALO4      EQU #44F2
_FPMULT         EQU #40AA
_FPADD          EQU #4092
_OP1SET0        EQU #4286
_SIN            EQU #40F6
_op1set1	EQU #4232
_op1set2	EQU #4242
_op1set3	EQU #423A
_op1set4	EQU #4236
_op2set0	EQU #4282
_op2set1	EQU #4252
_op2set2	EQU #4246
_op2set3	EQU #422E
_op2set4	EQU #422A
_op2set5	EQU #4222
_op2set8	EQU #421E
_op2set60	EQU #4EA8
_op2setA	EQU #4226
_op3set0	EQU #427E
_op3set1	EQU #421A
_op3set2	EQU #423E
_op4set0	EQU #427A
_op4set1	EQU #4216
_op5set0	EQU #4276
_pushop1	EQU #454A
_pushop3	EQU #4542
_popop1		EQU #44E6
_popop3		EQU #44E2
_setxxop1	EQU #4A74
_setxxop2	EQU #4A78
_setxxxxop2	EQU #4A7C
_dispop1a	EQU #51D4
_mov9toop1	EQU #4206
contrast	EQU #8008


grbuf		EQU plotsscreen
graph_mem	EQU plotsscreen
gbuf		EQU plotsscreen
sram		EQU saferam1
apdram		EQU saferam1
statram		EQU saferam2
statvars	EQU saferam2
vat		EQU progptr
saferam5	EQU imathptr1
_grbufcpy	EQU #4B9C
_grbufclr	EQU #515B
_clrlcdfull     EQU #4755
_clrscrnfull	EQU #475D
_chkfindsym     EQU #442A
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
