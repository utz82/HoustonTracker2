; Mallard include file
; (C) 2005, by Michael Vincent. All rights reserved.

LPON EQU #03
LPOFF EQU #00
LPL EQU #01
LPR EQU #02
LPSW EQU #03
LPSWL EQU #02
LMSK EQU #0303
LRMASK EQU #0102	;link port stereo mask
LPSWR EQU #01
LPORT EQU 0
KPORT EQU 1

userMem              equ  #9c37

	org userMem
	db $D9,$00,"Duck"
	dw begin


;See end of file for shell equates

;
;MACROS
;========================

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
MACRO AppOnErr,addr
	ld hl,addr
	call APP_PUSH_ERRORH
ENDM
MACRO AppOffErr
	call APP_POP_ERRORH
ENDM


;======================================================================
; Include File for the TI-73
; Last Updated 3/14/2005 (HAPPY PI DAY!)
;
; A few revisions and additional B_CALLs by Michael Vincent.
;
; Copyright (c) 2002 Texas Instruments: The Licensed Materials are
; copyrighted by TI. LICENSEE agrees that it will 
; not delete the copyright notice, trademarks or
; protective notices from any copy made by LICENSEE.
;
; Warranty: TI does not warrant that the Licensed Materials will
; be free from errors or will meet your specific requirements.
; The Licensed Materials are made available "AS IS" to LICENSEE.
;
; Limitations: TI MAKES NO WARRANTY OR CONDITION, EITHER EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
; REGARDING THE LICENSED MATERIALS.  IN NO EVENT SHALL
; TI OR ITS SUPPLIERS BE LIABLE FOR ANY INDIRECT, INCIDENTAL
; OR CONSEQUENTIAL DAMAGES, LOSS OF PROFITS, LOSS OF USE OR DATA, 
; OR INTERRUPTION OF BUSINESS, WHETHER THE ALLEGED DAMAGES ARE
; LABELED IN TORT, CONTRACT OR INDEMNITY.
;
;========================================================================


;SYMBOLIC_DEBUG_START
;=================================
;entry points
;=================================
_AbsO1O2Cp		equ #40f0
_AbsO1PAbsO2		equ #405a
_ACos			equ #40d2
_ACosRad		equ #40c6
_AdrLEle		equ #4ab6
_AdrMRow		equ #4567
_AdrMEle		equ #456a
_AllEq			equ #4792
_AllocFPS		equ #4369
_AllocFPS1		equ #436c
_AnsName		equ #4B67
_ApdSetup		equ #4c06
_AppGetCbl		equ #4b97
_AppGetCalc		equ #4b9a
_AppInit		equ #4048
_ASin			equ #40d8
_ASinRad		equ #40cf
_ATanRad		equ #40c9
_ATan2Rad		equ #40cc
_ATan			equ #40d5
_ATan2			equ #40db

_BinOPExec		equ #459a
_BufClear		equ #4840

_CanAlphIns		equ #4ab0
_ChkFindSym		equ #42d9
_CircCmd		equ #46f6
_CkInt			equ #423a
_CkOdd			equ #423d
_CkOP1C0		equ #422b
_CkOP1FP0		equ #422e
_CkOP1Pos		equ #4252
_CkOP1Real		equ #40e7
_CkOP2FP0		equ #4231
_CkOP2Pos		equ #424f
_CkOP2Real		equ #42ca
_CkPosInt		equ #4237
_CkValidNum		equ #426a
_CleanAll		equ #4927
_ClearRect		equ #4cfc
_ClearRow		equ #4c8d
_CLine			equ #46cc
_CLineS			equ #46cf
_CloseEditBuf		equ #47E9
_CloseEditBufNoR	equ #46a2
_CloseEditEqu		equ #4867
_CloseProg		equ #490c
_ClrGraphRef		equ #490f
_ClrLCD			equ #44aa
_ClrLCDFull		equ #44a7
_ClrLp			equ #41E3
_ClrOP1S		equ #4258
_ClrOP2S		equ #4255
_ClrScrn		equ #44b0
_ClrScrnFull		equ #44ad
_ClrTxtShd		equ #44b3
_CmpSyms		equ #4921
_ConvDim		equ #4B58
_ConvDim00		equ #4B5B
_ConvOP1		equ #49ab
_Cos			equ #40c0
_CpHLDE			equ #4003
_CPoint			equ #4d89
_CPointS		equ #4717
_CpOP1OP2		equ #40f3
_CpOP4OP3		equ #40ea
_CpyO1toFPS1		equ #4411
_CpyO1toFPS2		equ #4420
_CpyO1toFPS3		equ #442c
_CpyO1toFPS4		equ #443e
_CpyO1toFPS5		equ #4438
_CpyO1toFPS6		equ #4432
_CpyO1toFPS7		equ #4435
_CpyO1toFPST		equ #43ff
_CpyO2toFPS1		equ #440e
_CpyO2toFPS2		equ #4417
_CpyO2toFPS3		equ #4429
_CpyO2toFPS4		equ #443b
_CpyO2toFPST		equ #43f9
_CpyO3toFPS1		equ #4408
_CpyO3toFPS2		equ #441a
_CpyO3toFPST		equ #43f6
_CpyO5toFPS1		equ #440b
_CpyO5toFPS3		equ #4426
_CpyO6toFPS2		equ #441d
_CpyO6toFPST		equ #43fc
_CpyStack		equ #43de
_Cpyto1FPS1		equ #43e7
_Cpyto1FPS10		equ #43a8
_Cpyto1FPS11		equ #438d
_Cpyto1FPS2		equ #43f0
_Cpyto1FPS3		equ #43bd
_Cpyto1FPS4		equ #43c3
_Cpyto1FPS5		equ #4393
_Cpyto1FPS6		equ #4399
_Cpyto1FPS7		equ #439f
_Cpyto1FPS8		equ #43a2
_Cpyto1FPS9		equ #43ab
_Cpyto1FPST		equ #43d8
_CpyTo2FPS1		equ #43e4
_CpyTo2FPS2		equ #43ed
_CpyTo2FPS3		equ #43b7
_CpyTo2FPS4		equ #43ae
_CpyTo2FPS5		equ #4390
_CpyTo2FPS6		equ #4396
_CpyTo2FPS7		equ #439c
_CpyTo2FPS8		equ #43a5
_CpyTo2FPST		equ #43D5
_CpyTo3FPS1		equ #43e1
_CpyTo3FPS2		equ #43c6
_CpyTo3FPST		equ #43d2
_CpyTo4FPST		equ #43cf
_CpyTo5FPST		equ #43c9
_CpyTo6FPST		equ #43cc
_CpyTo6FPS3		equ #43b1
_CpyTo6FPS2		equ #43b4
_CpyToFPS1		equ #4414
_CpyToFPS2		equ #4423
_CpyToFPS3		equ #442f
_CpyToFPST		equ #4402
_CpyToStack		equ #4405
_Create0Equ		equ #4303
_CreateAppVar		equ #4312
_CreateEqu		equ #4309
_CreatePict		equ #430c
_CreateProg		equ #430f
_CreateReal		equ #42f4
_CreateRList		equ #42fa
_CreateStrng		equ #4300
_Cube			equ #407b
_CursorOff		equ #4522
_CursorOn		equ #4528

_DarkLine		equ #46ff
_DarkPnt		equ #4714
_DataSize		equ #433c
_DataSizeA		equ #4339
_DeallocFPS		equ #4363
_DeallocFPS1		equ #4366
_DecO1Exp		equ #4261
_DelListEl		equ #4906
_DelMem			equ #4330
_DelRes			equ #48fa
_DelVar			equ #432a
_DisableApd		equ #4be8
_DisableAPI		equ #4bdf
_DispDone		equ #4519
_DispEOL		equ #450a
_DispHL			equ #446e
_DisplayImage		equ #4d5c
_DispOP1A		equ #4AFB
_DivHLBy10		equ #4006
_DivHLByA		equ #4009
_DrawCirc2		equ #4aad
_DrawCmd		equ #47dd
_DrawRectBorder		equ #4d2c
_DrawRectBorderClear	equ #4d3b
_DrwRctBorderClr	equ #4d3b
_DToR			equ #4075

_EditProg		equ #4909
_EnableApd		equ #4beb
_EnoughMem		equ #42e5
_EOP1NotReal		equ #4273
_Equ_or_NewEqu		equ #42af
_EraseEOL		equ #44b9
_EraseRectBorder	equ #4d35
_ErrArgument		equ #49ff
_ErrBadGuess		equ #4a2f
_ErrBreak		equ #4a20
_ErrD_OP1_0		equ #42be
_ErrD_OP1_LE_0		equ #42bb
_ErrD_OP1Not_R		equ #42b5
_ErrD_OP1NotPos		equ #42b2
_ErrD_OP1NotPosInt	equ #42b8
_ErrDataType		equ #49f9
_ErrDimMismatch		equ #4a02
_ErrDimension		equ #4a05
_ErrDivBy0		equ #49e4
_ErrDomain		equ #49e7
_ErrIncrement		equ #49ea
_ErrInvalid		equ #4a11
_ErrIterations		equ #4a2c
_ErrLinkXmit		equ #4a38
_ErrMemory		equ #4a0b
_ErrNon_Real		equ #49ed
_ErrNonReal		equ #495a
_ErrNotEnoughMem	equ #4441
_ErrOverflow		equ #49e1
_ErrSignChange		equ #4a29
_ErrStat		equ #4a23
_ErrStatPlot		equ #4a35
_ErrSyntax		equ #49f0
_ErrTolTooSmall		equ #4a32
_ErrUndefined		equ #4a08
_EToX			equ #40b4
_ExLp			equ #4228
_ExpToHex		equ #4249

_FillRect		equ #4d02
_FillRectPattern	equ #4d38
_Find_Parse_Formula	equ #49b1
_FindAlphaDn		equ #491e
_FindAlphaUp		equ #491b
_FindSym		equ #42dc
_FiveExec		equ #45b5
_FixTempCnt		equ #4912
_FormEReal		equ #4885
_FormReal		equ #488B
_FourExec		equ #45b2
_FPAdd			equ #4072
_FPDiv			equ #4099
_FPMult			equ #4084
_FPRecip		equ #4096
_FPSquare		equ #4081
_FPSub			equ #406f
_Frac			equ #4093

_Get_Tok_Strng		equ #44F8
_GetBaseVer		equ #4b8e
_GetCSC			equ #400f
_GetKey			equ #486d
_GetLToOP1		equ #4573
_GetTokLen		equ #44f5
_GrBufClr		equ #4AD4
_GrBufCpy		equ #4789
_GrphCirc		equ #46f9

_HLTimes9		equ #40e4
_HorizCmd		equ #47c5
_HomeUp			equ #44bf
_HTimesL		equ #4270

_IBounds		equ #4aa4
_IBoundsFull		equ #4d4d
_ILine			equ #4702
_IncLstSize		equ #4900
_InsertList		equ #4903
_InsertMem		equ #42df
_Int			equ #40a5
_Intgr			equ #405d
_InvCmd			equ #47e3
_InvertRect		equ #4cff
_InvOP1S		equ #408d
_InvOP1SC		equ #408a
_InvOP2S		equ #4090
_InvSub			equ #4063
_IOffset		equ #4aa7
_IPoint			equ #4705
_IsA2ByteTok		equ #4291
_IsEditEmpty		equ #483A

_JError			equ #4a4d
_JErrorNo		equ #4c03
_JForceCmdNoChar	equ #401e

_KeyToString		equ #452e

_LCD_BUSY		equ #4051
_LCD_DRIVERON		equ #4873
_LdHLind		equ #4000
_LineCmd		equ #47cb
_LnX			equ #40ab
_Load_SFont		equ #46b7
_LoadPattern		equ #4c51
_LogX			equ #40ae

_Max			equ #4057
_MemChk			equ #42cd
_MemClear		equ #4a47
_MemSet			equ #4a4a
_Min			equ #4054
_Minus1			equ #406c
_Mov10B			equ #413e
_Mov18B			equ #46fc
_Mov9B			equ #4141
_Mov7B			equ #414a
_Mov8B			equ #4147
_Mov9ToOP1		equ #416b
_Mov9OP1OP2		equ #416e
_Mov9ToOP2		equ #4171
_MovFrOP1		equ #4174
_Mul_HL                 equ #4270

_NewLine		equ #4495

_OP1ExOP2		equ #4225
_OP1ExOP3		equ #421f
_OP1ExOP4		equ #4222
_OP1ExOP5		equ #4219
_OP1ExOP6		equ #421c
_OP1ExpToDec		equ #424c
_OP1Set0		equ #41d1
_OP1Set1		equ #4192
_OP1Set2		equ #419e
_OP1Set3		equ #4198
_OP1Set4		equ #4195
_OP1ToOP2		equ #4111
_OP1ToOP3		equ #4105
_OP1ToOP4		equ #40f9
_OP1ToOP5		equ #4135
_OP1ToOP6		equ #4132
_OP2Set0		equ #41ce
_OP2Set1		equ #41aa
_OP2Set2		equ #41a1
_OP2Set3		equ #418f
_OP2Set4		equ #418c
_OP2Set5		equ #4186
_OP2Set60		equ #497b
_OP2Set8		equ #4183
_OP2SetA		equ #4189
_OP2ToOP1		equ #4138
_OP2ToOP3		equ #415f
_OP2ToOP4		equ #40fc
_OP2ToOP5		equ #412c
_OP2ToOP6		equ #412f
_OP3Set0		equ #41cb
_OP3Set1		equ #4180
_OP3Set2		equ #419b
_OP3ToOP1		equ #4120
_OP3ToOP2		equ #4102
_OP3ToOP4		equ #40f6
_OP3ToOP5		equ #4129
_OP4Set0		equ #41c8
_OP4Set1		equ #417d
_OP4ToOP1		equ #411a
_OP4ToOP2		equ #40ff
_OP4ToOP3		equ #4162
_OP4ToOP5		equ #4126
_OP4ToOP6		equ #4168
_OP5Set0		equ #41c5
_OP5ToOP1		equ #411d
_OP5ToOP2		equ #4108
_OP5ToOP3		equ #4165
_OP5ToOP4		equ #410e
_OP5ToOP6		equ #410b
_OP6ToOP1		equ #4117
_OP6ToOP2		equ #4114
_OP6ToOP5		equ #4123

_ParseInp		equ #4966
_PDspGrph		equ #47c2
_PixelTest		equ #47d4
_Plus1			equ #4069
_PointCmd		equ #47d1
_PointOn		equ #4a56
_PopOP1			equ #4345
_PopOP3			equ #4342
_PopOP5			equ #433f
_PopReal		equ #435a
_PopRealO1		equ #4357
_PopRealO2		equ #4354
_PopRealO3		equ #4351
_PopRealO4		equ #434e
_PopRealO5		equ #434b
_PopRealO6		equ #4348
_PosNo0Int		equ #4234
_PToR			equ #40de
_PushOP1		equ #438a
_PushOP3		equ #4387
_PushOP5		equ #4384
_PushReal		equ #4381
_PushRealO1		equ #437e
_PushRealO2		equ #437b
_PushRealO3		equ #4378
_PushRealO4		equ #4375
_PushRealO5		equ #4372
_PushRealO6		equ #436f
_PutAway		equ #4039
_PutC			equ #446b
_PutMap			equ #4468
_PutPS			equ #4477
_PutPSB			equ #4474
_PutS			equ #4471
_PutTokString		equ #485e
_PutToL			equ #4582

_RandInit		equ #4c21
_Random			equ #4b9d
_Rcl_StatVar		equ #42c7
_RclAns			equ #4996
_RclGDB2		equ #46f3
_RclSysTok		equ #49a2
_RclVarSym		equ #499f
_RclX			equ #499c
_RclY			equ #4999
_RedimMat		equ #48fd
_Regraph		equ #47aa
_ReleaseBuffer		equ #46A5
_ReloadAppEntryVecs	equ #4a50
_RestoreDisp		equ #478c
_RndGuard		equ #409f
_RnFx			equ #40a2
_Round			equ #40a8
_RToD			equ #4078
_RToP			equ #40e1
_RunIndicOff		equ #44d7
_RunIndicOn		equ #44d4

_SaveDisp		equ #4ba3
_SendVarCmd		equ #48F1
_SetFuncM		equ #4762
_SetXXOP1		equ #46c0
_SetXXOP2		equ #46c3
_SetXXXXOP2		equ #46c6
_SFont_Len		equ #46ba
_ShRAcc			equ #41E6
_Sin			equ #40bd
_SinCosRad		equ #40ba
_SqRoot			equ #409c
_SrchVLstDn		equ #4c36
_SrchVLstUp		equ #4c33
_SStringLength		equ #4c54
_StMatEl		equ #49a5
_StoAns			equ #498a
_StoGDB2		equ #46f0
_StoOther		equ #4993
_StoRand		equ #4e5e
_StoSysTok		equ #4987
_StoX			equ #4990
_StoY			equ #498d
_StrCopy		equ #444a
_StrLength		equ #4a5c

_Tan			equ #40c3
_TenX			equ #40b7
_ThreeExec		equ #45ac
_Times2			equ #4066
_TimesPt5		equ #407e
_ToFrac			equ #4594
_Trunc			equ #4060

_UCLines		equ #46c9
_UnLineCmd		equ #47ce
_UnOpExec		equ #45a9

_VertCmd		equ #47c8
_VPutMap		equ #44c5
_VPutS			equ #44c8
_VPutSN			equ #44cb
_VtoWHLDE		equ #471d

_XftoI			equ #4726
_XitoF			equ #4720
_XName			equ #4279
_XRootY			equ #46d2

_YftoI			equ #4723
_YName			equ #427c
_YtoX			equ #46d5

_Zero16D		equ #41b9
_ZeroOP			equ #41e0
_ZeroOP1		equ #41d7
_ZeroOP2		equ #41da
_ZeroOP3		equ #41dd
_ZmDecml		equ #476e
_ZmFit			equ #477a
_ZmInt			equ #476b
_ZmPrev			equ #4771
_ZmSquare		equ #477d
_ZmStats		equ #46d8
_ZmTrig			equ #4780
_ZmUsr			equ #4774
_ZooDefault		equ #4786

;======================================================================

;           system-defined ram variable address equates

;======================================================================

;


appData              equ  #8000
ramStart             equ  #8000
ramCode              equ  #8100
ramCodeEnd           equ  #822f
baseAppBrTab         equ  #8230
bootTemp             equ  #8241
appSearchPage        equ  #8293
tempSwapArea         equ  #8295
appID                equ  #837d
kbdScanCode          equ  #83d8
kbdKey               equ  #83dd
kbdGetKy             equ  #83de
keyExtend            equ  #83df
contrast             equ  #83e0
curTime              equ  #83e1
curRow               equ  #83e2
curCol               equ  #83e3
curOffset            equ  #83e4
curUnder             equ  #83e5
ioFlag               equ  #83e6
sndRecState          equ  #83e8
ioErrState           equ  #83e9
header               equ  #83ea
ioData               equ  #83f3
bakHeader            equ  #83ff
OP1                  equ  #842b
OP1M                 equ  #842d
OP2                  equ  #8436
OP2EXT               equ  #843f
OP3                  equ  #8441
OP3M                 equ  #8443
OP4                  equ  #844c
OP4M                 equ  #844e
OP5                  equ  #8457
OP5M                 equ  #8459
OP6                  equ  #8462
OP6M                 equ  #8464
OP6EXT               equ  #846b
progToEdit           equ  #8472
nameBuff             equ  #847a
equ_edit_save        equ  #8485
OP2M                 equ  #8485
iMathPtr1            equ  #8486
iMathPtr2            equ  #8488
iMathPtr3            equ  #848a
iMathPtr4            equ  #848c
iMathPtr5            equ  #848e
chkDelPtr1           equ  #8490
chkDelPtr2           equ  #8492
insDelPtr            equ  #8494
upDownPtr            equ  #8496
curY                 equ  #8498
curType              equ  #8499
curXRow              equ  #849a
prevDData            equ  #849b
lFont_record         equ  #84a3
sFont_record         equ  #84ab
menuActive           equ  #84b3
menuAppDepth         equ  #84b4
MenuCurrent          equ  #84b5
ProgCurrent          equ  #84c1
rclQueue             equ  #84d2
rclQueueEnd          equ  #84d4
indicMem             equ  #84d6
indicCounter         equ  #84de
indicBusy            equ  #84df
apdSubTimer          equ  #84e0
apdTimer             equ  #84e1
cxMain               equ  #84e2
cxPPutAway           equ  #84e4
cxPutAway            equ  #84e6
cxRedisp             equ  #84e8
cxErrorEP            equ  #84ea
cxSizeWind           equ  #84ec
cxPage               equ  #84ee
cxCurApp             equ  #84ef
cxPrev               equ  #84f0
monQH                equ  #850e
monQT                equ  #850f
monQueue             equ  #8510
onSP                 equ  #8520
ramReturnData        equ  #8529
errNo                equ  #854a
errSP                equ  #854b
errOffset            equ  #854d
bstCounter           equ  #8559
flags                equ  #855b
tokVarPtr            equ  #85a1
tokLen               equ  #85a3
fOutDat              equ  #85a5
asm_data_ptr1        equ  #85af
asm_data_ptr2        equ  #85b1
asm_sym_ptr1         equ  #85b3
asm_sym_ptr2         equ  #85b5
asm_ind_call         equ  #85b7
appRawKeyHandle      equ  #85b8
appBank_jump         equ  #85c9
appPage              equ  #85cb
customHeight         equ  #8612
appBackUpScreen      equ  #8613
textShadow           equ  #8913
textShadCur          equ  #8993
textShadTop          equ  #8995
textShadAlph         equ  #8996
textShadIns          equ  #8997
appInfo              equ  #8998
promptRow            equ  #89a2
promptCol            equ  #89a3
promptIns            equ  #89a4
promptShift          equ  #89a5
promptRet            equ  #89a6
promptValid          equ  #89a8
promptTop            equ  #89aa
promptCursor         equ  #89ac
promptTail           equ  #89ae
promptBtm            equ  #89b0
varType              equ  #89b2
varCurrent           equ  #89b3
varClass             equ  #89bb
penCol               equ  #89df
penRow               equ  #89e0
userMenuSA           equ  #89e1
ioPrompt             equ  #8a41
dImageWidth          equ  #8a42
RectFillPHeight      equ  #8A42
RectFillPWidth       equ  #8a43
RectFillPattern      equ  #8a44
saveSScreen          equ  #8a52
statVars             equ  #8d58
curGStyle            equ  #8e94
curGY                equ  #8e95
curGX                equ  #8e96
curGY2               equ  #8e97
curGX2               equ  #8e98
freeSaveY            equ  #8e99
freeSaveX            equ  #8e9a
XOffset              equ  #8ec8
YOffset              equ  #8ec9
lcdTallP             equ  #8eca
pixWideP             equ  #8ecb
pixWide_m_1          equ  #8ecc
pixWide_m_2          equ  #8ecd
lastEntryPTR         equ  #8ece
lastEntryStk         equ  #8ed0
numLastEntries       equ  #90d0
currLastEntry        equ  #90d1
curInc               equ  #92de
uXmin                equ  #92f5
uXmax                equ  #92fe
uXscl                equ  #9307
uYmin                equ  #9310
uYmax                equ  #9319
uYscl                equ  #9322
uThetMin             equ  #932b
uThetMax             equ  #9334
uThetStep            equ  #933d
uTmin                equ  #9346
uTmax                equ  #934f
uTStep               equ  #9358
uPlotStart           equ  #9361
unMax                equ  #936a
uu0                  equ  #9373
uv0                  equ  #937c
unMin                equ  #9385
uu02                 equ  #938e
uv02                 equ  #9397
uw0                  equ  #93a0
uPlotStep            equ  #93a9
uXres                equ  #93b2
uw02                 equ  #93bb
Xmin                 equ  #93c7
Xmax                 equ  #93d0
Xscl                 equ  #93d9
Ymin                 equ  #93e2
Ymax                 equ  #93eb
Yscl                 equ  #93f4
ThetaMin             equ  #93fd
ThetaMax             equ  #9406
ThetaStep            equ  #940f
TminPar              equ  #9418
TmaxPar              equ  #9421
Tstep                equ  #942a
PlotStart            equ  #9433
nMax                 equ  #943c
u0                   equ  #9445
v0                   equ  #944e
nMin                 equ  #9457
u02                  equ  #9460
v02                  equ  #9469
w0                   equ  #9472
PlotStep             equ  #947b
XresO                equ  #9484
w02                  equ  #948d
smallEditRAM         equ  #949f
XFact                equ  #950b
YFact                equ  #9514
Xres_int             equ  #951d
deltaX               equ  #951e
deltaY               equ  #9527
shortX               equ  #9530
shortY               equ  #9539
lower                equ  #9542
upper                equ  #954b
XOutSym              equ  #9558
XOutDat              equ  #955a
YOutSym              equ  #955c
YOutDat              equ  #955e
inputSym             equ  #9560
inputDat             equ  #9562
prevData             equ  #9564
TblMin               equ  #9676
TblStep              equ  #967f
P1Type               equ  #968c
SavX1List            equ  #968d
SavY1List            equ  #9692
SavF1List            equ  #9697
P1FrqOnOff           equ  #96ab
P2Type               equ  #96ae
SavX2List            equ  #96af
SavY2List            equ  #96b4
SavF2List            equ  #96b9
P2FrqOnOff           equ  #96cd
P3Type               equ  #96d0      
SavX3List            equ  #96d1
SavY3List            equ  #96d6
SavF3List            equ  #96d8
P3FrqOnOff           equ  #96ef
plotSScreen          equ  #9737
seed1                equ  #9a37
seed2                equ  #9a40
cmdCursor            equ  #9a7e
editTop              equ  #9a82
editCursor           equ  #9a84
editTail             equ  #9a86
editBtm              equ  #9a88
editSym              equ  #9a94
editDat              equ  #9a96
winTop               equ  #9b33
winBtm               equ  #9b34
winLeftEdge          equ  #9b35
winLeft              equ  #9b36
winAbove             equ  #9b38
winRow               equ  #9b3a
winCol               equ  #9b3c
fmtDigits            equ  #9b3e
fmtString            equ  #9b3f
fmtConv              equ  #9b80
fmtLeft              equ  #9b92
fmtIndex             equ  #9b94
fmtMatSym            equ  #9b96
fmtMatMem            equ  #9b98
EQS                  equ  #9b9a
tSymPtr1             equ  #9ba6
tSymPtr2             equ  #9ba8
chkDelPtr3           equ  #9baa
chkDelPtr4           equ  #9bac
tempMem              equ  #9bae
fpBase               equ  #9bb0
FPS                  equ  #9bb2
OPBase               equ  #9bb4
OPS                  equ  #9bb6
pTempCnt             equ  #9bb8
cleanTmp             equ  #9bba
pTemp                equ  #9bbc
progPtr              equ  #9bbe
newDataPtr           equ  #9bc0
appErr1              equ  #9bc2
appErr2              equ  #9bcf

symTable             equ  #fe66

;SYMBOLIC_DEBUG_END
;====================================
; equates for fraction entry routines
;====================================
fieldMax    equ    OP5
fieldLen    equ    fieldMax+1
fieldCol    equ    fieldLen+1     ; pencol of start
fieldRow    equ    fieldCol+1     ; penrow of start  3 rows above char
fieldInput  equ    fieldRow+1     ; 9 bytes,
wholeMax    equ    tempSwapArea
wholeLen    equ    wholeMax+1
wholeCol    equ    wholeLen+1     ; pencol of start
wholeRow    equ    wholeCol+1     ; penrow of start  3 rows above char
wholeInput  equ    wholeRow+1     ; 9 bytes,
;
numerMax    equ    wholeInput+9
numerLen    equ    numerMax+1
numerCol    equ    numerLen+1     ; pencol of start
numerRow    equ    numerCol+1     ; penrow of start
numerInput  equ    numerRow+1     ; 9 bytes,
;
denomMax    equ    numerInput+9
denomLen    equ    denomMax+1
denomCol    equ    denomLen+1     ; pencol of start
denomRow    equ    denomCol+1     ; penrow of start
denomInput  equ    denomRow+1     ; 9 bytes,
;
fracLineLen equ   denomInput+9
fracLineCol equ   fracLineLen+1
;
fracRightEdge  equ fracLineCol+1
;
curPosition equ   fracRightEdge+1
;
numerSpaces equ   curPosition+1
denomSpaces equ   numerSpaces+1
haveFrac    equ   3
haveUnit    equ   4
incCurPos   equ   5


;
;======================================================================
;           system variable equates
;======================================================================
;       entry points : rclsystok, stosystok
;
;       vars with no restriction
;
XSCLt       equ  3           ; 'xscl'           4
YSCLt       equ  4           ; 'yscl'           4
;
;       vars rounded to 10 digits
;
XMINt       EQU  #0A         ; 'xMin'           4
XMAXt       EQU  #0B         ; 'xMax'           4
YMINt       EQU  #0C         ; 'yMin'           4
YMAXt       EQU  #0D         ; 'yMax'           4
TBLMINt     EQU  #1A         ; 'TBLMIN'
;
;       rouned to 10 digits, and positve integers
;
PLOTSTARTt   equ  #1b         ; 'nstart'         5
UPLOTSTARTt  equ  #1c         ; 'znstart'        6

;       vars cannot be 0
;
TBLSTEPt    EQU  #21         ; 'TBLSTEP'
TSTEPt      EQU  #22         ; 'tStep'          5
THETSTEPt   EQU  #23         ; Ltheta,'Step'    5
UTSTEPt     EQU  #24         ; 'ztStep'         6
UTHSTEPt    EQU  #25         ; 'z',Ltheta,'Step' 6
;
;       special limits on these
;
DELTAXt     EQU  #26         ; LcapDelta,'x'    2
DELTAYt     EQU  #27         ; LcapDelta,'y'    2
;
;       special limits on these
;
XFACTt      EQU  #28         ; 'xFact'          5
YFACTt      EQU  #29         ; 'yFact'          5
;
;       sys var list
;
TBLINPUTt       equ       #2a
;
;
;
PLOTSTEPt       equ       #34
UPLOTSTEPt      equ       #35
;
XRESt           equ       #36
UXRESt          equ       #37
;
; special token used by input/prompt for storing value to variable

INPUTt      equ  #40         ; set up for store for input/prompt

; special token used by graph math intersection routine to indicate
; list data

ISECTt      equ  #41         ; list data follows this
;
;========== automatic power down time out value (<= 7 minutes)
apdTimeOut	equ	5		;automatic power down time (minutes)

;========== interrupt equate
iAll 	equ 1011b

;========== i/o ports
intrptEnPort	equ 3 ; interrupt enable (in/out) 
memPageAPort	equ	6		; memory page a	(i/o)
LCDInstPort     equ     #10
LCDDataPort     equ     #11 


;========== common subroutine rst numbers
rOP1TOOP2	EQU  #08
rFINDSYM	EQU  #10
rPUSHREALO1	EQU  #18
rMOV9TOOP1	EQU  #20
rFPADD		EQU  #30

;============error handler equates/macros
APP_PUSH_ERRORH EQU	#59
APP_POP_ERRORH	EQU	#5C

;========== run indicators

busyPause	equ	%10101010
busynNormal	equ	%11110000

;======================================================================
;           system and state flags
;======================================================================
;

trigFlags	equ  0		;trigonometry mode settings
trigDeg 	equ    2	; 1=degrees, 0=radians

kbdFlags	equ  0		;keyboard scan
kbdScr		equ    3	; 1=scan code ready
kbdKeyPress	equ    4	; 1=key has been pressed

doneFlags	equ  0		;display "done"
donePrgm	equ    5	; 1=display "done" after prgm

ioDelFlag	equ  0
inDelete	equ    0	;1 = delete screen 

;----------------------------------------------------------------------
editFlags	equ  1
editOpen	equ    2	; 1=edit buffer is open

monFlags	equ  1		;monitor flags
monAbandon	equ    4	; 1=don't start any long process
				; in put away (#715)
;----------------------------------------------------------------------
plotFlags	equ  2		;plot generation flags
plotTrace	equ  0
plotLoc 	equ    1	; 0=bkup & display, 1=display only
plotDisp	equ    2	; 1=plot is in display, 0=text in display


grfModeFlags	equ  2		;graph mode settings
grfFuncM	equ    4	; 1=function graph
grfPolarM	equ    5	; 1=polar graph
grfParamM	equ    6	; 1=parametric graph
grfRecurM       equ    7        ; 1=recursion graph

graphFlags	equ  3
graphProg	equ  1     ; 1=graph in progress
graphDraw	equ    0	; 0=graph is valid, 1=redraw graph

grfDBFlags	equ  4
grfDot		equ    0	; 0=line, 1=dot
grfSimul	equ    1	; 0=sequential, 1=simultaneous
grfGrid 	equ    2	; 0=no grid, 1=grid
grfPolar	equ    3	; 0=rectangular, 1=polar coordinates
grfNoCoord	equ    4	; 0=display coordinates, 1=off
grfNoAxis	equ    5	; 0=axis, 1=no axis
grfLabel	equ    6	; 0=off, 1=axis label

textFlags	equ  5		;text output flags
textEraseBelow	equ    1	; 1=erase line below small char
textScrolled	equ    2	; 1=screen scrolled
textInverse	equ    3	; 1=display inverse bit-map
textInsMode	equ    4	; 0=overstrike, 1=insert mode

parsFlag2	equ  7		;parser flags
numOP1		equ    0	; 1=result in op1, 0=no result

newDispF        equ  8		;derivative mode flags
preClrForMode   equ    0	; 1=help blink on mode screen

apdFlags	equ  8		;automatic power-down
apdAble 	equ    2	; 1=apd enabled
apdRunning	equ    3	; 1=apd clock running


web_err_mask    equ  #60

onFlags 	equ  9		;on key flags
onRunning	equ	 3
onInterrupt	equ    4	; 1=on key interrupt request

statFlags	equ  9		;statistics flags
statsValid	equ    6	; 1=stats are valid

fmtFlags	equ  10		;numeric format flags
fmtExponent	equ    0	; 1=show exponent, 0=no exponent
fmtEng		equ    1	; 1=engineering notion, 0=scientific
fmtHex		equ    2	; 1=hexadecimal
fmtOct		equ    3	; 1=octal
fmtBin		equ    4	; 1=binary
;
numMode         equ  10
fmtReal         equ    5
fmtRect         equ    6
fmtPolar        equ    7

realMode        equ    5
rectMode        equ    6
polarMmode       equ    7
;					;   if hex and oct both = 1
;					;   then bin=0 means >frac
;					;	 bin=1 means >dms
fmtBaseMask     equ  %00011100		; mask to base flags
fmtBaseShift    equ  2			; offset to base flags
;
;       check if these are used by numform,
;
;               equ  6
;               equ  7

fmtOverride	equ  11		;copy of fmtflags with conversion override

fmtEditFlags	equ  12		;numeric editing flags
fmtEdit 	equ    0	; 1=format number for editing

curFlags	equ  12		;cursor
curAble 	equ    2	; 1=cursor flash is enabled
curOn		equ    3	; 1=cursor is showing
curLock 	equ    4	; 1=cursor is locked off

appFlags	equ  13		;application flags
appWantIntrpt	equ    0		; 1=want on key interrupts
appTextSave	equ    1		; 1=save characters in textshadow
appAutoScroll	equ    2		; 1=auto-scroll text on last line
appMenus	equ    3		; 1=process keys that bring up menus
					; 0=check lock menu flag
appLockMenus	equ    4		; 1=ignore menu keys
					; 0=switch to home screen and bring up menu
appCurGraphic	equ    5		; 1=graphic cursor
appCurWord	equ    6		; 1=text cursor covers entire word
appExit 	equ    7		; 1=application handles [exit] key itself

appWantIntrptF	equ	1<<appWantIntrpt
appTextSaveF	equ	1<<appTextSave
appAutoScrollF	equ	1<<appAutoScroll
appMenusF	equ	1<<appMenus
appLockMenusF	equ	1<<appLockMenus
appCurGraphicF	equ	1<<appCurGraphic
appCurWordF	equ	1<<appCurWord
appExitF	equ	1<<appExit


seqFlags        equ  15              ; sequential graph flags
webMode         equ    0             ; 0 = normal seq mode, 1 = web mode
webVert         equ    1             ;
sequv           equ    2             ; u vs v
seqvw           equ    3             ; v vs w
sequw           equ    4             ; u vs w


promptFlags	equ  17		;prompt line flags
promptEdit	equ    0	; 1=editing in prompt buffer

indicFlags	equ  18		;indicator flags
indicRun	equ    0	; 1=run indicator on
indicInUse	equ    1	; indicator save area in use=1, free=0
indicOnly	equ    2	; interrupt handler only checks run indicator

shiftFlags	equ  18		;[2nd] and [alpha] flags
shift2nd	equ    3	; 1=[2nd] has been pressed
shiftAlpha	equ    4	; 1=[alpha] has been pressed
shiftLwrAlph	equ    5	; 1=lower case, 0=upper case
shiftALock	equ    6	; 1=alpha lock has been pressed
shiftKeepAlph	equ    7	; 1=cannot cancel alpha shift


tblFlags        equ  19		;table flags.
autoFill        equ    4	; 1=prompt, 0=fillauto
autoCalc        equ    5	; 1=prompt, 0=calcauto
reTable         equ    6	; 0=table is ok, 1=must recompute table.


fracMode        equ      20             ;FRAC FLAGS
ABCF            equ       0             ; a b/c if 1, b/c if 0.
AutoSimpF       equ       1             ; autosimp if 0, mansimp if 1.
ManSimpF        equ       1             ; autosimp if 0, mansimp if 1.

ConstMode       equ     20
Hide            equ       2            ;0 if show, 1 if hide.
Multi           equ       3            ;0 if single, 1 if multiple.

sGrFlags	equ  20
write_on_graph  equ    4	; 1 = text or equ writing to graph screen
g_style_active  equ    5	; 1 = graph styles are enabled, use them
cmp_mod_box     equ    6	; 1 = doing mod box plot computation
textWrite       equ    7
;

newIndicFlags   equ  21
extraIndic      equ    0
saIndic         equ    1

smartFlags      equ  23
;
;----------------------------------------------------------------------
; note: fix these equates if smartflags are moved                            
;----------------------------------------------------------------------
;
smarter_mask    equ  3
smarter_test    equ  1
smartGraph      equ  0
smartGraph_inv  equ  1                                                 

;----------------------------------------------------------------------
;           available for asm programming
;----------------------------------------------------------------------

asm_Flag1       equ  33          ; asm coding
asm_Flag2       equ  34          ; asm coding
asm_Flag3       equ  35          ; asm coding

;----------------------------------------------------------------------

fracFlags	equ	  36		; fraction display/edit
fracStopEOS	equ	   0
fracInside	equ	   1
fracDontDraw	equ	   2
fracEditMode	equ	   3
fracInNum	equ	   4
fracScrScrolled equ	   5
fracFlag1	equ	   6
fracClearEOS	equ	   7	   ; tell drawExpr code to clear to end of screen when done
;
fontFlags	equ	   37
fracDrawNum	equ	   0
fracDrawDenom	equ	   1
fracDrawLFont	equ	   2
fracTallLFont	equ	   3
fracLastChar	equ	   4
fracCursor	equ	   5
DrawLfont       equ       6
CustomFont      equ       7
;
;flags for fraction math routines:
FracFlags2      equ     38
DtoF10F         equ      0      ;10digit flag for Dec ->Frac
SimpKeyF        equ      1      ;Used for Simp K.
RatD1K          equ      1      ;also used to show denominator=1000
FinalType       equ      2      ;1=final type is improper 0=type is simple
FinalFrac       equ      3      ;set if final fraction result expected.
ManDtoF         equ      4      ;manual simp=1, Dec->frac bit.
ManForceS       equ      5      ;manual simp=1, Abc=0; force simplify anyhow.
ManAdjW         equ      6      ;manual simp=1, Abc=0; Adjust whole flag.
ManBin          equ      7      ;manual simp=1, Binop set.


;flags for fraction math routines:
FracFlags3      equ     39
FNormOver       equ      0      ;Fraction Normalization Override if set.
DivSmallNum     equ      1      ;set if Frac Numerator < denominator.
IntDivDisp      equ      2      ;integer divide display flag.
StrictInt       equ      3      ;strict integer check by intdiv if set.
yxRatF          equ      4      ;y^x rational approximation flag.
nid             equ      5      ;1 if non integer decimal
SimpDisp        equ      6      ;1 if need to display frac simplify factor.
HadTrueMixed    equ      7      ;at least one true fraction in an expression.
;(HadTrueMixed is needed by >Ab/c<>d/e so that if any mixed fraction, then
; if convert, will stay b/c if b/c mode & not convert back to mixed.)

;
getSendFlg	equ  47
comFailed	equ  3	      ; 1 = get/send communication failed
;
;
apiFlg3		equ  52 
;
apiFlg4		equ  53
fullScrnDraw	equ    2	; draw into last row/col of screen

xapFlag0        equ  43      ; external app flags
xapFlag1        equ  44
xapFlag2        equ  45
xapFlag3        equ  46

plotFlag3	equ  47  
bufferOnly	equ    6
useFastCirc	equ    4

;
varTypeMask	equ  #1f
varGraphRef	equ    6

;======================================================================
;           character font equates  
;======================================================================
;
LrecurN         equ     #001
LrecurU 	equ	#002
LrecurV 	equ	#003
LrecurW		equ	#004
Lconvert	equ	#005
LsqUp		equ	#006
LsqDown		equ	#007
Lintegral	equ	#008
Lcross		equ	#009
LboxIcon 	equ     #00a
LcrossIcon 	equ     #00b
LdotIcon 	equ     #00c
LsubT   	equ     #00d		;small capital t for parametric mode.
LcubeR  	equ     #00e		;slightly different 3 for cubed root.
LhexF		equ	#00f
Lroot		equ	#010
Linverse	equ	#011
Lsquare		equ	#012
Langle		equ	#013
Ldegree		equ	#014
Lradian		equ	#015
Ltranspose	equ	#016
LLE		equ	#017
LNE		equ	#018
LGE		equ	#019
Lneg		equ	#01a
Lexponent	equ	#01b
Lstore		equ	#01c
Lten		equ	#01d
LupArrow	equ	#01e
LdownArrow	equ	#01f
Lspace		equ	#020
Lexclam		equ	#021
Lquote		equ	#022
Lpound		equ	#023
Lfourth		equ     #024
Lpercent	equ	#025
Lampersand	equ	#026
Lapostrophe	equ	#027
LlParen		equ	#028
LrParen		equ	#029
Lasterisk	equ	#02a
LplusSign	equ	#02b
Lcomma		equ	#02c
Ldash		equ	#02d
Lperiod		equ	#02e
Lslash		equ	#02f
L0		equ	#030
L1		equ	#031
L2		equ	#032
L3		equ	#033
L4		equ	#034
L5		equ	#035
L6		equ	#036
L7		equ	#037
L8		equ	#038
L9		equ	#039
Lcolon		equ	#03a
Lsemicolon	equ	#03b
LLT		equ	#03c
LEQ		equ	#03d
LGT		equ	#03e
Lquestion	equ	#03f
LatSign		equ	#040
LcapA	equ	#041
LcapB	equ	#042
LcapC	equ	#043
LcapD	equ	#044
LcapE	equ	#045
LcapF	equ	#046
LcapG	equ	#047
LcapH	equ	#048
LcapI	equ	#049
LcapJ	equ	#04a
LcapK	equ	#04b
LcapL	equ	#04c
LcapM	equ	#04d
LcapN	equ	#04e
LcapO	equ	#04f
LcapP	equ	#050
LcapQ	equ	#051
LcapR	equ	#052
LcapS	equ	#053
LcapT	equ	#054
LcapU	equ	#055
LcapV	equ	#056
LcapW	equ	#057
LcapX	equ	#058
LcapY	equ	#059
LcapZ	equ	#05a
Ltheta  equ     #05b
Lbackslash	equ	#05c
LrBrack		equ	#05d
Lcaret		equ	#05e
Lunderscore	equ	#05f
Lbackquote	equ	#060
La	equ	#061
Lb	equ	#062
Lc	equ	#063
;Ld	equ	#064
;Le	equ	#065
Lf	equ	#066
Lg	equ	#067
Lh	equ	#068
Li	equ	#069
Lj	equ	#06a
Lk	equ	#06b
Ll	equ	#06c
Lm	equ	#06d
Ln	equ	#06e
Lo	equ	#06f
Lp	equ	#070
Lq	equ	#071
Lr	equ	#072
Ls	equ	#073
;Lt	equ	#074
Lu	equ	#075
Lv	equ	#076
Lw	equ	#077
Lx	equ	#078
Ly	equ	#079
Lz	equ	#07a
LlBrace	equ	#07b
Lbar	equ	#07c
LrBrace	equ	#07d
Ltilde	equ	#07e
LinvEQ	equ	#07f
Lsub0	equ	#080
Lsub1	equ	#081
Lsub2	equ	#082
Lsub3	equ	#083
Lsub4	equ	#084
Lsub5	equ	#085
Lsub6	equ	#086
Lsub7	equ	#087
Lsub8	equ	#088
Lsub9	equ	#089
LcapAAcute	equ	#08a
LcapAGrave	equ	#08b
LcapACaret	equ	#08c
LcapADier	equ	#08d
LaAcute		equ	#08e
LaGrave		equ	#08f
LaCaret		equ	#090
LaDier		equ	#091
LcapEAcute	equ	#092
LcapEGrave	equ	#093
LcapECaret	equ	#094
LcapEDier	equ	#095
LeAcute		equ	#096
LeGrave		equ	#097
LeCaret		equ	#098
LeDier		equ	#099
LcapIAcute	equ	#09a
LcapIGrave	equ	#09b
LcapICaret	equ	#09c
LcapIDier	equ	#09d
LiAcute		equ	#09e
LiGrave		equ	#09f
LiCaret		equ	#0a0
LiDier		equ	#0a1
LcapOAcute	equ	#0a2
LcapOGrave	equ	#0a3
LcapOCaret	equ	#0a4
LcapODier	equ	#0a5
LoAcute		equ	#0a6
LoGrave		equ	#0a7
LoCaret		equ	#0a8
LoDier		equ	#0a9
LcapUAcute	equ	#0aa
LcapUGrave	equ	#0ab
LcapUCaret	equ	#0ac
LcapUDier	equ	#0ad
LuAcute		equ	#0ae
LuGrave		equ	#0af
LuCaret		equ	#0b0
LuDier		equ	#0b1
LcapCCed	equ	#0b2
LcCed		equ	#0b3
LcapNTilde	equ	#0b4
LnTilde		equ	#0b5
Laccent		equ	#0b6
Lgrave		equ	#0b7
Ldieresis	equ	#0b8
LquesDown	equ	#0b9
LexclamDown	equ	#0ba
Lalpha		equ	#0bb
Lbeta		equ	#0bc
Lgamma		equ	#0bd
LcapDelta	equ	#0be
Ldelta		equ	#0bf
Lepsilon	equ	#0c0
LlBrack 	equ     #0c1
Llambda		equ	#0c2
Lmu		equ	#0c3
Lpi		equ	#0c4
Lrho		equ	#0c5
LcapSigma	equ	#0c6
Lsigma		equ	#0c7
Ltau		equ	#0c8
Lphi		equ	#0c9
LcapOmega	equ	#0ca
LxMean		equ	#0cb
LyMean		equ	#0cc
LsupX		equ	#0cd
Lellipsis	equ	#0ce
Lleft		equ	#0cf
Lblock		equ	#0d0
Lper		equ	#0d1
Lhyphen		equ	#0d2
Larea		equ	#0d3
Ltemp		equ	#0d4
Lcube		equ	#0d5
Lenter		equ	#0d6
LimagI		equ	#0d7
Lphat		equ	#0d8
Lchi		equ	#0d9
LstatF		equ     #0da
Llne		equ	#0db
LlistL		equ	#0dc
LfinanN 	equ	#0dd
L2_r_paren 	equ	#0de
LblockArrow     equ     #0df
LcurO   	equ     #0e0
LcurO2  	equ     #0e1
LcurOcapA       equ     #0e2
LcurOa  	equ     #0e3
LcurI   	equ     #0e4
LcurI2  	equ     #0e5
LcurIcapA       equ     #0e6
LcurIa  	equ     #0e7
LGline          equ     #0e8    ; = 0
LGthick         equ     #0e9    ; = 1
LGabove         equ     #0ea    ; = 2
LGbelow         equ     #0eb    ; = 3
LGpath          equ     #0ec    ; = 4
LGanimate       equ     #0ed    ; = 5
LGdot           equ     #0ee    ; = 6
LGpBlk          equ     #0ef    ;up arrow and block in solver
LDnBlk          equ     #0f0    ;down arrow and block in solver
LcurFull        equ     #0f1    ;note: must be last char (putmap checks)

;========= floating point

FPLEN           equ     9               ;length of a floating-point number

;========= constant table equates

drConst 	equ	0
pi2Const	equ	1
pi4Const	equ	2
logeConst	equ	3
piConst 	equ	4
DtoRConst	equ	5
ln10		equ	6

;========== text screen
textRows	equ	8
textCols        equ     16

;========== alphatext screen
alphabuffersize equ	16
;
;========== msc fraction equates
mantSignBit     equ  7       ;msbit of data type byte is set if negative.
redF            equ  1       ;reduced if 0, unreduced if 1.
unredF          equ  1       ;reduced if 0, unreduced if 1.
simpF           equ  0       ;simple if 0, mixed if 1.
mixedF          equ  0       ;simple if 0, mixed if 1.
;

;
;

;====== keyboard key names
;
;       key presses that do not echo into the display
;       and are not application or menu loading key presses
;
kRight          EQU       #001
kLeft           EQU       #002
kUp             EQU       #003
kDown           EQU       #004
kEnter          EQU       #005
kAlphaEnter     EQU       #006
kAlphaUp        EQU       #007
kAlphaDown      EQU       #008
kClear          EQU       #009
kDel            EQU       #00A
kIns            EQU       #00B
kRecall         EQU       #00C
kLastEnt        EQU       #00D
kBOL            EQU       #00E
kEOL            EQU       #00F
;
kSelAll         EQU       #010
kUnselAll       EQU       #011
kLtoTI82        EQU       #012
kBackup         EQU       #013
kRecieve        EQU       #014
kLnkQuit        EQU       #015
kTrans          EQU       #016
kRename         EQU       #017
kOverw          EQU       #018
kOmit           EQU       #019
kCont           equ       #01A
kLtoTI83        equ       #01B
kSendApp        equ       #01C
kvApps          equ       #01C
kSendId         equ       #01D
kSendSW         equ       #01E
kvALL           equ       #01F
kvSendType      equ       #020  ;send a particular data type (in kextend)
kNoWay          EQU       #021
kYes            EQU       #022
kAbout          equ       #023  ;about screen
kApp            equ       #024
;
kProgExec       equ       #025
kListflag       equ       #026
;
;open for expansion       028h..02bh
;

menustart       equ       #02c
;
;       key presses that do not echo into the display
;       and are menu switching key presses
;
kAreYouSure     equ       #02C
kPrgm           equ       #02D
kZoom           equ       #02E        ; PULL DOWN
kDraw           equ       #02F
kSPlot          equ       #030
kStat           equ       #031
kMath           equ       #032
kTest           equ       #033
kAppsMenu       equ       #034
kVars           equ       #035
kMem            equ       #036
kConstSetmenu   equ       #037
kConvMenu       equ       #038
kTrigmenu       equ       #039
kLIST           equ       #03A
kConstMenu      equ       #03B
;
menuend         equ       kConstMenu
;
kConst          EQU       #03C
;
;
kAlpha          equ       #03d
kCatalog        equ       #03e        ; catalog
kInputDone      equ       #3f

;
;       key presses that do not echo into the display
;       and are context switching key presses
;
kQuit           equ       #040
kLinkIO         equ       #041
kChkApps        equ       #042        ; Check Apps
kStatEd         equ       #043
kGraph          equ       #044
kMode           equ       #045
kPrgmEd         equ       #046        ; PROGRAM EDIT
kPrgmCr         equ       #047        ; PROGRAM CREATE
kWindow         equ       #048        ; RANGE EDITOR
kYequ           equ       #049        ; EQUATION EDITOR
kTable          equ       #04A        ; TABLE EDITOR
kTblSet         equ       #04B        ; TABLE SET
kChkRAM         equ       #04C        ; CHECK RAM
kDelMem         equ       #04D        ; DELETE MEM
kResetMem       equ       #04E        ; RESET MEM
kResetDef	equ	  #04F        ; RESET DEFAULT
kPrgmInput      EQU       #050        ; PROGRAM INPUT
kZFactEd        EQU       #051        ; ZOOM FACTOR EDITOR
kError          EQU       #052        ; ERROR
kExtApps        EQU       #053        ; Extra Applications.     NEW
kSolveRoot	equ	  #054        ; SOLVE EDITOR
kStatP          equ       #055        ; stat plot
kConstEd        equ       #056        ; Constant equ Editor.
kPrgmSetVar     equ       #057        ; SetMenu in a Program.
kFormat         equ       #058        ; FORMAT
;
;
append          equ       kFormat
;
;       58h - 59h are empty for growth
;
echoStart1      equ       #05a
;
;       conditional keys that if in prog edit will echo
;       or if not in prog edit go to the graph screen
;       'trace' , 'zooms'
;
;
kTrace          equ       #05a
kZFit           equ       #05b
kZIn            equ       #05c
kZOut           equ       #05d
kZPrev          equ       #05e
kZBox           equ       #05f
kZDecml         equ       #060
kSetZm          equ       #061
kZSquar         equ       #062
kZStd           equ       #063
kZTrig          equ       #064
kZQuad1         equ       #065
;kusrzm          equ       065h
;kzsto           equ       066h
kZInt           equ       #067
kZStat          equ       #068
;
;       conditional keys that if in prog edit or if the
;       graph screen is not displayed will echo, else will
;       go to the graph screen. 'draw' menu, eval
echoStart2      equ       #069
;
;
kSelect         equ       #069
kCircl          equ       #06a
kClDrw          equ       #06b
kLine           equ       #06c
kPen            equ       #06d
kPtChg          equ       #06e
kPtOff          equ       #06f
kPtOn           equ       #070
kVert           equ       #071
kHoriz          equ       #072
kText           equ       #073
kTanLn          equ       #074
kManFit         equ       #075
;
;   these keys will always force to graph mode
;
kEval           equ       #076
;
echoStart       equ       #07d
;
;       key presses that echo into a buffer
;
kRemain         equ       #07d          ;only needed to display 'r'
kPlot3          equ       #07e
kListName       equ       #07f
kAdd            equ       #080
kSub            equ       #081
kMul            equ       #082
kDiv            equ       #083
kExpon          equ       #084
kLParen         equ       #085
kRParen         equ       #086
kLBrack         equ       #087
kRBrack         equ       #088
kShade          equ       #089
kStore          equ       #08a
kComma          equ       #08b
kChs            equ       #08c
kDecPnt         equ       #08d
k0              equ       #08e
k1              equ       #08f
k2              equ       #090
k3              equ       #091
k4              equ       #092
k5              equ       #093
k6              equ       #094
k7              equ       #095
k8              equ       #096
k9              equ       #097
kEE             equ       #098
kSpace          equ       #099
kCapA           EQU       #09A
kCapB           EQU       #09B
kCapC           EQU       #09C
kCapD           EQU       #09D
kCapE           EQU       #09E
kCapF           EQU       #09F
kCapG           EQU       #0A0
kCapH           EQU       #0A1
kCapI           EQU       #0A2
kCapJ           EQU       #0A3
kCapK           EQU       #0A4
kCapL           EQU       #0A5
kCapM           EQU       #0A6
kCapN           EQU       #0A7
kCapO           EQU       #0A8
kCapP           EQU       #0A9
kCapQ           EQU       #0AA
kCapR           EQU       #0AB
kCapS           EQU       #0AC
kCapT           EQU       #0AD
kCapU           EQU       #0AE
kCapV           EQU       #0AF
kCapW           EQU       #0B0
kCapX           EQU       #0B1
kCapY           EQU       #0B2
kCapZ           EQU       #0B3
kVarx           EQU       #0B4
kPi             EQU       #0B5
kInv            EQU       #0B6
kSin            EQU       #0B7
kASin           EQU       #0B8
kCos            EQU       #0B9
kACos           EQU       #0BA
kTan            EQU       #0BB
kATan           EQU       #0BC
kSquare         EQU       #0BD
kSqrt           EQU       #0BE
kLn             EQU       #0BF
kExp            EQU       #0C0
kLog            EQU       #0C1
kALog           EQU       #0C2
kToABC          EQU       #0C3
;
;kclrtbl         equ       #0c4h
;
kAns            equ       #0c5
kColon          equ       #0c6
;

kUnit           equ       #0c7          ;                       new
kFracSlash      equ       #0c8          ;                       new
kRoot           equ       #0c9
;
kQuest          equ       #0ca        ; question mark
kQuote          equ       #0cb        ; "
kTheta          equ       #0cc
kIf             equ       #0cd
kThen           equ       #0ce
kElse           equ       #0cf
kFor            equ       #0d0
kWhile          equ       #0d1
kRepeat         equ       #0d2
kEnd            equ       #0d3
kPause          equ       #0d4
kLbl            equ       #0d5
kGoto           equ       #0d6
kISG            equ       #0d7
kDSL            equ       #0d8
kMenu           equ       #0d9
kExec           equ       #0da
kReturn         equ       #0db
kStop           equ       #0dc
kInput          equ       #0dd
kPrompt         equ       #0de
kDisp           equ       #0df
kDispG          equ       #0e0
kDispT          equ       #0e1
kOutput         equ       #0e2
kGetKey         equ       #0e3
kClrHome        equ       #0e4
;
;kprtscr        equ       #0e5
;
kPercent        equ       #0e6         ;%                      new
kMixSimp        equ       #0e7         ;                       new
kFracDec        equ       #0e8         ;                       new
kSimp           equ       #0e9         ;                       new
kUnredF         equ       #0ea         ;                       new
kIDiv           equ       #0eb         ;                       new
kConvert        equ       #0ec          ;                       new
kLBrace         equ       #0ed
kRBrace         equ       #0ee
kL1A            equ       #0ef
kL2A            equ       #0f0
kL3A            equ       #0f1
kL4A            equ       #0f2
kL5A            equ       #0f3
kL6A            equ       #0f4
kConvLength     equ       #0f5        ;1st byte of conversion key.
kConvArea       equ       #0f6        ;1st byte of conversion key.
kConvVolume     equ       #0f7        ;1st byte of conversion key.
kConvTime       equ       #0f8        ;1st byte of conversion key.
kConvTemp       equ       #0f9        ;1st byte of conversion key.
kConvMass       equ       #0fa        ;1st byte of conversion key.
kConvSpeed      equ       #0fb        ;1st byte of conversion key.
;
;       this key means that it is a 2 byte keycode
;       there are 2 of these keys be careful
;
;
kExtendEcho2    equ       #0fc
;
;
;       this key means that the key press is one that echos
;       into a buffer, and it is a 2 byte key code, go look at
;       (extecho) for the key value
;
kExtendEcho     equ       #0fe
;
kE1BT           equ       0

kDrawInv        equ       kE1BT
kDrawF          equ       kE1BT+1
kPixelOn        equ       kE1BT+2
kPixelOff       equ       kE1BT+3
kPxlTest        equ       kE1BT+4
kRCGDB          equ       kE1BT+5
kRCPic          equ       kE1BT+6
kSTGDB          equ       kE1BT+7
kSTPic          equ       kE1BT+8
kAbs            equ       kE1BT+9
kTEqu           equ       kE1BT+10    ; ==
kTNoteQ         equ       kE1BT+11    ; <>
kTGT            equ       kE1BT+12    ; >
kTGTE           equ       kE1BT+13    ; > =
kTLT            equ       kE1BT+14    ; <
kTLTE           equ       kE1BT+15    ; < =

kAnd            equ       kE1BT+16
kOr             equ       kE1BT+17
kXor            equ       kE1BT+18
kNot            equ       kE1BT+19

;klr1            equ       kE1BT+20

kXRoot          equ       kE1BT+21
kCube           equ       kE1BT+22
kCbRt           equ       kE1BT+23    ; cube root
kToDec          equ       kE1BT+24
;
kCubicR         equ       kE1BT+25
kQuartR         equ       kE1BT+26
;
kPlot1          equ       kE1BT+27
kPlot2          equ       kE1BT+28
;

kRound          equ       kE1BT+29
kIPart          equ       kE1BT+30
kFPart          equ       kE1BT+31
kInt            equ       kE1BT+32

kRand           equ       kE1BT+33
kNPR            equ       kE1BT+34
kNCR            equ       kE1BT+35
kXFactorial     equ       kE1BT+36

kRad            equ       kE1BT+37
kDegr           equ       kE1BT+38    ; degrees conv
kApost          equ       kE1BT+39
kToDMS          equ       kE1BT+40
kRtoPo          equ       kE1BT+41    ; r
kRtoPr          equ       kE1BT+42
kPtoRx          equ       kE1BT+43
kPtoRy          equ       kE1BT+44

kRowSwap        equ       kE1BT+45
kRowPlus        equ       kE1BT+46
kTimRow         equ       kE1BT+47
kTRowP          equ       kE1BT+48

kSortA          equ       kE1BT+49
kSortD          equ       kE1BT+50
kSeq            equ       kE1BT+51

kMin            equ       kE1BT+52
kMax            equ       kE1BT+53
kMean           equ       kE1BT+54
kMedian         equ       kE1BT+55
kSum            equ       kE1BT+56
kProd           equ       kE1BT+57

kDet            equ       kE1BT+58
kTransp         equ       kE1BT+59
kDim            equ       kE1BT+60    ; same as one in list
kFill           equ       kE1BT+61    ; same as one in list
kIdent          equ       kE1BT+62
kRandm          equ       kE1BT+63
kAug            equ       kE1BT+64

kLstMode        equ       kE1BT+65
;available      equ       kE1BT+66

kmRad           equ       kE1BT+67
kmDeg           equ       kE1BT+68
kmNormF         equ       kE1BT+69
kmSci           equ       kE1BT+70
kFix            equ       kE1BT+71
kmFloat         equ       kE1BT+72
kSetMenu        equ       kE1BT+73
kEOFrac         equ       kE1BT+74

kBoyIcon        equ     kE1BT+75
kTreeIcon       equ     kE1BT+76
kFaceIcon       equ     kE1BT+77
kCircleIcon     equ     kE1BT+78
kDiamondIcon    equ     kE1BT+79
kStarIcon       equ     kE1BT+80
kDollarIcon      equ    kE1BT+81
;available      equ     kE1BT+82
;available      equ     kE1BT+83

kFNOn           equ       kE1BT+84
kFNOff          equ       kE1BT+85

kPlotsOn        equ       kE1BT+86
kPlotsPff       equ       kE1BT+87

kPixelChg       equ       kE1BT+88

kSendMBL        equ       kE1BT+89
kRecvMBL        equ       kE1BT+90

kBoxPlot        equ       kE1BT+ 91
kBoxIcon        equ       kE1BT+ 92

kCrossIcon      equ       kE1BT+ 93
kDotIcon        equ       kE1BT+ 94

kE2BT           equ       kE1BT+95

kSeqential      equ       kE2BT         ;105
kSimulG         equ       kE2BT+1       ;106
kCoordOn        equ       kE2BT+2       ;107
kCoordOff       equ       kE2BT+3       ;108
kDrawLine       equ       kE2BT+4       ;109
kDrawDot        equ       kE2BT+5       ;110
kAxisOn         equ       kE2BT+6       ;111
kAxisOff        equ       kE2BT+7       ;112
kGridOn         equ       kE2BT+8       ;113
kGridOff        equ       kE2BT+9       ;114
kLblOn          equ       kE2BT+10      ;115
kOblOff         equ       kE2BT+11      ;116
kPolarG         equ       kE2BT+12      ;117
kRectG          equ       kE2BT+13      ;118

kL1             equ       kE2BT+14      ;119 = 77h
kL2             equ       kE2BT+15
kL3             equ       kE2BT+16
kL4             equ       kE2BT+17
kL5             equ       kE2BT+18
kL6             equ       kE2BT+19

kMatA          equ       kE2BT+20
kMatB           equ       kE2BT+21
kMatC           equ       kE2BT+22
kMatD           equ       kE2BT+23
kMatE           equ       kE2BT+24

kXmin           EQU       kE2BT+25
kXmax           EQU       kE2BT+26
kXscl           EQU       kE2BT+27
kYmin           EQU       kE2BT+28
kYmax           EQU       kE2BT+29
kYscl           EQU       kE2BT+30
kTmin           EQU       kE2BT+31
kTmax           EQU       kE2BT+32
kTStep          EQU       kE2BT+33
kOmin           EQU       kE2BT+34
kOmax           EQU       kE2BT+35
kOStep          EQU       kE2BT+36
ku0             EQU       kE2BT+37
kv0             EQU       kE2BT+38
knMin           EQU       kE2BT+39
knMax           EQU       kE2BT+40
kDeltaY         EQU       kE2BT+41
kDeltaX         EQU       kE2BT+42

kZXmin          EQU       kE2BT+43
kZXmax          EQU       kE2BT+44
kZXscl          EQU       kE2BT+45
kZYmin          EQU       kE2BT+46
kZYmax          EQU       kE2BT+47
kZYscl          EQU       kE2BT+48
kZTmin          EQU       kE2BT+49
kZTmax          EQU       kE2BT+50
kZTStep         EQU       kE2BT+51
kZOmin          EQU       kE2BT+52
kZOmax          EQU       kE2BT+53
kZOStep         EQU       kE2BT+54
;
kABC         equ       kE2BT+55
kBC           equ       kE2BT+56
kAutoSimp       equ       kE2BT+57
kManSimp        equ       kE2BT+58

kDelLast        equ       kE2BT+59
ksingleC        equ       kE2BT+60
kMultiC         equ       kE2BT+61

kPic1           equ       kE2BT+62
kPic2           equ       kE2BT+63
kPic3           equ       kE2BT+64

kDelVar         equ       kE2BT+65
kGetCalc        equ       kE2BT+66

kStndrd         equ       kE2BT+67
kParam          equ       kE2BT+68
kPolar          equ       kE2BT+69
kAFillOn        equ       kE2BT+70
kAFillOff       equ       kE2BT+71
kACalcOn        equ       kE2BT+72
kACalcOff       equ       kE2BT+73

kSetConst       equ       kE2BT+74
;
kGraphStyle     equ       kE2BT+75
;
kExprOn         EQU       kE2BT+76
kExprOff        EQU       kE2BT+77
kStatA          EQU       kE2BT+78
kStatB          EQU       kE2BT+79
kStatC          EQU       kE2BT+80
kCorr           EQU       kE2BT+81
kStatD          EQU       kE2BT+82
kStatE          EQU       kE2BT+83
kRegEq          EQU       kE2BT+84
kMinX           EQU       kE2BT+85
kQ1             EQU       kE2BT+86
kMD             EQU       kE2BT+87
kQ3             EQU       kE2BT+88
kMaxX           EQU       kE2BT+89
kStatX1         EQU       kE2BT+90
kStatY1         EQU       kE2BT+91
kStatX2         EQU       kE2BT+92
kStatY2         EQU       kE2BT+93
kStatX3         EQU       kE2BT+94
kStatY3         EQU       kE2BT+95
kTblMin         EQU       kE2BT+96
kTblStep        EQU       kE2BT+97
kSetupLst       EQU       kE2BT+98
kClrAllLst      EQU       kE2BT+99
kLogistic       EQU       kE2BT+100
kc1             equ       kE2BT+101   ;constant c1
kc2             equ       kE2BT+102   ;constant c2
kc3             equ       kE2BT+103   ;constant c3
kc4             equ       kE2BT+104   ;constant c4
kPlotStart      equ       kE2BT+105   ; plotstart
kZPlotStart     equ       kE2BT+106   ; zplotstart
kXFact          equ       kE2BT+107   ; xfact
kYFact          equ       kE2BT+108   ; yfact
kMaxY           equ       kE2BT+109   ; maxy
kTblInput       equ       kE2BT+110   ; web off
kPlotStep       equ       kE2BT+111
kZPlotStep      equ       kE2BT+112
kSelectA        equ       kE2BT+113
kZFitA          equ       kE2BT+114
kOneVar         equ       kE2BT+115
kTwoVar         equ       kE2BT+116
kLR             equ       kE2BT+117
kLRExp          equ       kE2BT+118
kLRLn           equ       kE2BT+119
;klrpwr          equ       kE2BT+
;kmanfit         equ       kE2BT+
kMedMed         equ       kE2BT+120
kQuad           equ       kE2BT+121
kClrLst         equ       kE2BT+122
kHist           equ       kE2BT+123
kXYLine         equ       kE2BT+124
kScatter        equ       kE2BT+125
kLR1            equ       kE2BT+126
kClrTbl         equ       kE2BT+127
kE2BT_end       equ       kClrTbl
;
;

; new 2 byte keys

kE2BT2		EQU       0

kGDB1		EQU       kE2BT2
kGDB2		EQU       kE2BT2+1
kGDB3		EQU       kE2BT2+2
kY1		EQU       kE2BT2+3
kY2		EQU       kE2BT2+4
kY3		EQU       kE2BT2+5
kY4		EQU       kE2BT2+6
kY5		EQU       kE2BT2+7
kY6		EQU       kE2BT2+8
kY7		EQU       kE2BT2+9
kY8		EQU       kE2BT2+10
kY9		EQU       kE2BT2+11
kY0		EQU       kE2BT2+12
kX1T		EQU       kE2BT2+13
kY1T		EQU       kE2BT2+14
kX2T		EQU       kE2BT2+15
kY2T		EQU       kE2BT2+16
kX3T		EQU       kE2BT2+17
kY3T		EQU       kE2BT2+18
kX4T		EQU       kE2BT2+19
kY4T		EQU       kE2BT2+20
kX5T		EQU       kE2BT2+21
kY5T		EQU       kE2BT2+22
kX6T		EQU       kE2BT2+23
kY6T		EQU       kE2BT2+24
kR1		EQU       kE2BT2+25
kR2		EQU       kE2BT2+26
kR3		EQU       kE2BT2+27
kR4		EQU       kE2BT2+28
kR5		EQU       kE2BT2+29
kR6		EQU       kE2BT2+30
kGDB4		EQU       kE2BT2+31
kGDB5		EQU       kE2BT2+32
kGDB6		EQU       kE2BT2+33
kPic4		EQU       kE2BT2+34
kPic5		EQU       kE2BT2+35
kPic6		EQU       kE2BT2+36
kGDB7		EQU       kE2BT2+37
kGDB8		EQU       kE2BT2+38
kGDB9		EQU       kE2BT2+39
kGDB0		EQU       kE2BT2+40
kPic7		EQU       kE2BT2+41
kPic8		EQU       kE2BT2+42
kPic9		EQU       kE2BT2+43
kPic0		EQU       kE2BT2+44
kStatN		EQU       kE2BT2+45
kXMean		EQU       kE2BT2+46
;kconj           equ       kE2BT2+47
;kreal           equ       kE2BT2+48
;kfangle         equ       kE2BT2+49
kLCM            equ       kE2BT2+50
kGCD            equ       kE2BT2+51
kRandInt        equ       kE2BT2+52
kCoin           equ       kE2BT2+53
kDice           equ       kE2BT2+54
kRemainder      equ       kE2BT2+55
kYMean          equ       kE2BT2+56
kStdX           equ       kE2BT2+57
kStdX1          equ       kE2BT2+58
kw0             equ       kE2BT2+59
kMatf           equ       kE2BT2+60
kMatg           equ       kE2BT2+61
kMatrh          equ       kE2BT2+62
kMati           equ       kE2BT2+63
kMatj           equ       kE2BT2+64
kYMean1         equ       kE2BT2+65
kStdY           equ       kE2BT2+66
kStdY1          equ       kE2BT2+67
kMaTtoLst       equ       kE2BT2+68
kLstToMat       equ       kE2BT2+69
kCumSum         equ       kE2BT2+70
kDeltaLst       equ       kE2BT2+71
kStdDev         equ       kE2BT2+72
kVariance       equ       kE2BT2+73
kLength         equ       kE2BT2+74
kEquToStrng     equ       kE2BT2+75
kStrngToEqu     equ       kE2BT2+76
kExpr           equ       kE2BT2+77
kSubStrng       equ       kE2BT2+78
kInStrng        equ       kE2BT2+79
kStr1           equ       kE2BT2+80
kStr2           equ       kE2BT2+81
kStr3           equ       kE2BT2+82
kStr4           equ       kE2BT2+83
kStr5           equ       kE2BT2+84
kStr6           equ       kE2BT2+85
kStr7           equ       kE2BT2+86
kStr8           equ       kE2BT2+87
kStr9           equ       kE2BT2+88
kStr0           equ       kE2BT2+89
kSimpFact       equ       kE2BT2+90
kPictPlot       equ     kE2BT2+91
kCirclePlot     equ     kE2BT2+92
kStemPlot       equ     kE2BT2+93
kBarPlot        equ     kE2BT2+94
kSumX           equ       kE2BT2+102
kSumX2          equ       kE2BT2+103

kSumY           equ       kE2BT2+104
kXRes           equ       kE2BT2+105

kSumY2          equ       kE2BT2+106
kSumXY          equ       kE2BT2+107
kuXRes          equ       kE2BT2+108
kModBox         equ       kE2BT2+109
kNormProb       equ       kE2BT2+110

kMinY           equ       kE2BT2+111   ; miny
kRef            equ       kE2BT2+112
kRRef           equ       kE2BT2+113
kLRSqr          equ       kE2BT2+114
kBRSqr          equ       kE2BT2+115
kDiagOn         equ       kE2BT2+116
kDiagOff        equ       kE2BT2+117
;kun1            equ       kE2BT2+152   ; for rcl use when gotten from 82
;kvn1            equ       kE2BT2+153   ; for rcl use when gotten from 82
kE2BT2_end      equ       kDiagOff

;=================================================
;    context equates
;=================================================
cxCmd           equ     kQuit                   ;home screen
cxAppsApp       equ     kChkApps                ;apps usage screen
cxPrgmEdit      equ     kPrgmEd                 ;program editor
cxEquEdit       equ     kYequ                   ;equation editor
cxGrRange       equ     kWindow                 ;graph range editor
cxGrZfact       equ     kZFactEd                ;graph zoom factors editor
cxGraph         equ     kGraph                  ;graph mode
cxStatEdit      equ     kStatEd                 ;statistics list editor
cxPrgmInput     equ     kPrgmInput              ;programmed input
cxError         equ     kError                  ;error handler
cxLinkIO        equ     kLinkIO                 ;link i/o interface
cxMem           equ     kResetMem               ;reset memory
cxDefMem        equ     kResetDef               ;reset default
cxRAMApp        equ     kChkRAM                 ;ram usage screen
cxMode          equ     kMode                   ;mode settings screen
cxErase         equ     kDelMem                 ;memory erase
cxPrgmCreate    equ     kPrgmCr                 ;program create
cxTableEditor   equ     kTable                  ;table editor
cxTableSet      equ     kTblSet                 ;table set up
cxStatPlot      equ     kStatP                  ;stat plots
cxFormat        equ     kFormat                 ;format context
cxSolveRoot     equ     kSolveRoot              ;solve root
cxextapps       equ     kExtApps        ;053h   ; extra applications.     new
cxConstEd       equ     kConstEd        ;056h   ; constant equ editor.
cxPrgmSetVar    equ     kPrgmSetVar     ;057h   ; setmenu in a program.

;============================================
;	scan code equates
;============================================

skDown          EQU	#01
skLeft          EQU	#02
skRight         EQU	#03
skUp            EQU	#04
skEnter         EQU	#09
skAdd           EQU	#0A
skSub           EQU	#0B
skMul           EQU	#0C
skDiv           EQU	#0D
skConst         equ              #0e
skClear         equ              #0f
skChs           equ              #11
sk3             equ              #12
sk6             equ              #13
sk9             equ              #14
skRParen        equ              #15
skMixSimp       equ              #16
skAppsMenu      equ              #17
skDecPnt        equ              #19
sk2             equ              #1a
sk5             equ              #1b
sk8             equ              #1c
skLParen        equ              #1d
skFracDec       equ              #1e
skPrgm          equ              #1f
skStatEd        equ              #20
sk0             equ              #21
sk1             equ              #22
sk4             equ              #23
sk7             equ              #24
skPercent       equ              #25
skFracSlash     equ              #26
skExpon         equ              #27
skDraw          equ              #28
skStore         equ              #2a
skComma         equ              #2b
skVarX          equ              #2c
skSimp          equ              #2d
skUnit          equ              #2e
skSquare        equ              #2f
skMath          equ              #30
skGraph         equ              #31
skTrace         equ              #32
skZoom          equ              #33
skWindow        equ              #34
skYEqu          equ              #35
sk2nd           equ              #36
skMode          equ              #37
skDel           equ              #38

;==========================================
;	tokens
;==========================================

EOSSTART        EQU       0
;
;
;   DISPLAY CONVERSIONS COME IMMEDIATELY BEFORE 'TSTORE'
;
DCONV   EQU     01h
;
tToDMS          EQU       DCONV       ; 01h
tToDEC          EQU       DCONV+1     ; 02h
tToAbc          EQU       DCONV+2     ; 03h    > A b/c
tUnredF         equ     03h     ;down arrow for unreduced fraction token

tStore          EQU       DCONV+3     ; 04h Lstore                01
;
tBoxPlot        EQU       05h
;
BRACKS          EQU       06h
;
tLBrack         EQU       BRACKS      ; 06h '['
tRBrack         EQU       BRACKS+1    ; 07h ']'
tLBrace         EQU       BRACKS+2    ; 08h '{'
tRBrace         EQU       BRACKS+3    ; 09h '}'

tPOST1          EQU       BRACKS+4
;
tFromRad        EQU       tPOST1       ; 0Ah Lradian
tFromDeg        EQU       tPOST1+1     ; 0Bh Ldegree
tRecip          EQU       tPOST1+2     ; 0Ch Linverse
tSqr            EQU       tPOST1+3     ; 0Dh Lsquare
tTrnspos        EQU       tPOST1+4     ; 0Eh Ltranspose
tCube           EQU       tPOST1+5     ; 0Fh '^3'
;
tLParen         EQU       #10         ; 10h '('
tRParen         EQU       #11         ; 11h ')'

;   immediate unary tokens go from here to 'eosel-1'
;
IMUN            EQU       #12
;
tRound          EQU       IMUN        ; 12h 'round'
tPxTst          EQU       IMUN+1      ; 13h 'PXL-TEST'
tAug            EQU       IMUN+2      ; 14h 'aug'
tRowSwap        EQU       IMUN+3      ; 15h 'rSwap'
tRowPlus        EQU       IMUN+4      ; 16h 'rAdd'
tmRow           EQU       IMUN+5      ; 17h 'multR'
tmRowPlus       EQU       IMUN+6      ; 18h 'mRAdd'
tMax            EQU       IMUN+7      ; 19h 'max'
tMin            EQU       IMUN+8      ; 1Ah 'min'
tRToPr          EQU       IMUN+9      ; 1Bh 'R>Pr
tRToPo          EQU       IMUN+10     ; 1Ch 'R>Po
tPToRx          EQU       IMUN+11     ; 1Dh 'P>Rx
tPToRy          EQU       IMUN+12     ; 1Eh 'P>Ry
tMedian         EQU       IMUN+13     ; 1Fh 'MEDIAN
tRandM          EQU       IMUN+14     ; 20h 'randM'
tMean           EQU       IMUN+15     ; 21h
tLstMode        equ       IMUN+16     ; 22h 'mode('
;the below tokens are immediate, but require an expression as 1st arg & so
;are grouped together...
tRoot           equ       IMUN+17     ; 23h 'solve('
tSeries         equ       IMUN+18     ; 24h 'seq'
tEvalF          equ       IMUN+20     ; 26h             ;keep until sure.
IMUNEND         equ     IMUN+20         ;end of immediate unaries.

tSolveEqual     equ     IMUN+21         ;teq ('=') in solver -> tsolveequal.
tEOFrac         equ     IMUN+22         ; 28h end of fraction in home screen.
;
;
;  token 29h cannot be an eos function since el(=29h already
;
tEOSEL          EQU       IMUN+23
;
tSpace          EQU       tEOSEL       ; 29h   ' '
tString         EQU       tEOSEL+1     ; 2Ah  '"'
tComma          EQU       tEOSEL+2     ; 2Bh  ','
tRemain         equ     2ch     ;'r' for remainder display.
; tremain is needed in home screen where turn display chars into tokens.
; (there is no token for small 'r').
; ===== postfix functions
;
tPost           equ       #2e
;
tFact           equ       tPost       ; 2eh '!'
tPercent        equ     tPost+1       ; 2fh '%'                 new

;=============number tokens

NUMS            EQU       30h
;
t0              EQU       NUMS        ; 30h
t1              EQU       NUMS+1      ; 31h
t2              EQU       NUMS+2      ; 32h
t3              EQU       NUMS+3      ; 33h
t4              EQU       NUMS+4      ; 34h
t5              EQU       NUMS+5      ; 35h
t6              EQU       NUMS+6      ; 36h
t7              EQU       NUMS+7      ; 37h
t8              EQU       NUMS+8      ; 38h
t9              EQU       NUMS+9      ; 39h
tDecPt          EQU       NUMS+10     ; 3Ah
tee             EQU       NUMS+11     ; 3Bh

;=======================
;   binary op
;=======================
tOr             EQU       #3C         ; 3Ch  '_or_'
tXor            EQU       #3D         ; 3Dh
;
tColon          EQU       #3E         ; 3Eh ':'
tEnter          EQU       #3F         ; 3Fh  Lenter
;
tAnd            EQU       #40         ; 40h  '_and_'       

;========================
;       letter tokens
;========================
LET             EQU       #41
;
tA              EQU       LET         ; 41h
tB              EQU       LET+1       ; 42h
tC              EQU       LET+2       ; 43h
tD              EQU       LET+3       ; 44h
tE              EQU       LET+4       ; 45h
tF              EQU       LET+5       ; 46h
tG              EQU       LET+6       ; 47h
tH              EQU       LET+7       ; 48h
tI              EQU       LET+8       ; 49h
tJ              EQU       LET+9       ; 4Ah
tK              EQU       LET+10      ; 4Bh
tL              EQU       LET+11      ; 4Ch
tM              EQU       LET+12      ; 4Dh
tN              EQU       LET+13      ; 4Eh
tO              EQU       LET+14      ; 4Fh
tP              EQU       LET+15      ; 50h
tQ              EQU       LET+16      ; 51h
tR              EQU       LET+17      ; 52h
tS              EQU       LET+18      ; 53h
tT              EQU       LET+19      ; 54h
tU              EQU       LET+20      ; 55h
tV              EQU       LET+21      ; 56h
tW              EQU       LET+22      ; 57h
tX              EQU       LET+23      ; 58h
tY              EQU       LET+24      ; 59h
tZ              EQU       LET+25      ; 5Ah
tEOF            equ       LET+26      ; 5bh
;
;===============================================
;   these var tokens are 1st of a double token
;===============================================
;
;======================================================================
;   THESE VAR TOKENS ARE 1ST OF A DOUBLE TOKEN 
;======================================================================

;
vToks   EQU     LET+27    ;
;
;       USER MATRIX TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarMat         EQU       vToks       ; 5Ch
;
;       USER LIST TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarLst         EQU       vToks+1     ; 5Dh
;
;       USER EQUATION TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarEqu         EQU       vToks+2     ; 5Eh
tProg           EQU       vToks+3     ; 5Fh
;
;       USER PICT TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarPict        EQU       vToks+4     ; 60h
;
;       USER GDB TOKEN, 2ND TOKEN NEEDED FOR NAME
;
tVarGDB         EQU       vToks+5     ; 61h
tVarOut         EQU       vToks+6     ; 62h
tVarSys         EQU       vToks+7     ; 63h

;================================
; ===== mode setting commands
;================================
MODESA          EQU       vToks+8     ; 64h
;
tRad            EQU       MODESA      ; 64h 'Radian'
tDeg            EQU       MODESA+1    ; 65h 'Degree'
tNormF          EQU       MODESA+2    ; 66h 'Normal'
tSci            EQU       MODESA+3    ; 67h 'Sci'
tEng            EQU       MODESA+4    ; 68h 'Eng'
tFloat          EQU       MODESA+5    ; 69h 'Float'
;
CMPS            EQU       #6A
;
tEQ             EQU       CMPS        ; 6Ah '=='
tLT             EQU       CMPS+1      ; 6Bh '<'
tGT             EQU       CMPS+2      ; 6Ch '>'
tLE             EQU       CMPS+3      ; 6Dh LLE
tGE             EQU       CMPS+4      ; 6Eh LGE
tNE             EQU       CMPS+5      ; 6Fh LNE

;=======================
;   binary op
;=======================
tAdd            equ       #70         ; 70h '+'
tSub            equ       #71         ; 71h '-'
;
tAns            equ       #72         ; 72h
;
;============================
;===== mode setting commands
;============================
;       graph format tokens are 2 byte tokens
;
tGFormat        equ     #73
;
;
tBoyIcon        equ     #78           ; for stat plots in programs
tTreeIcon       equ     #79           ; for stat plots in programs
tDollarIcon     equ     #7a           ; for stat plots in programs
tFaceIcon       equ     #7b           ; for stat plots in programs
tCircleIcon     equ     #7c           ; for stat plots in programs
tDiamondIcon    equ     #7d           ; for stat plots in programs
tStarIcon       equ     #7e           ; for stat plots in programs
tBoxIcon        equ       #7f
tCrossIcon      equ       #80
tDotIcon        equ       #81
;
;===========================
;       binary op cont.
;===========================
tMul            equ       #82         ; 82h  '*'
tDiv            equ       #83         ; 83h  '/'
tIDiv           equ       #84         ; 84h  'int/'
;
;============================
;       some graph commands
;============================
;
GRCMDS          equ       85h
;
tTrace          EQU       GRCMDS      ; 84h 'Trace'
tClDrw          EQU       GRCMDS+1    ; 85h 'ClDrw'
tZoomStd        EQU       GRCMDS+2    ; 86h 'ZStd'
tZoomtrg        EQU       GRCMDS+3    ; 87h 'Ztrg'
tZoomBox        EQU       GRCMDS+4    ; 88h 'ZBOX'
tZoomIn         EQU       GRCMDS+5    ; 89h 'ZIn'
tZoomOut        EQU       GRCMDS+6    ; 8Ah 'ZOut'
tZoomSqr        EQU       GRCMDS+7    ; 8Bh 'ZSqr'
tZoomInt        EQU       GRCMDS+8    ; 8Ch 'ZInt'
tZoomPrev       EQU       GRCMDS+9    ; 8Dh 'ZPrev'
tZoomDec        EQU       GRCMDS+10   ; 8Eh 'ZDecm'
tZoomStat       EQU       GRCMDS+11   ; 8Fh 'ZStat
tZoomQuad1      equ       GRCMDS+12   ; 91h 'quadrant 1'
tZoomSto        equ       GRCMDS+14   ; 93h  zoom store
tText           equ       GRCMDS+15   ; 94h
;
;=======================
;       binary op
;=======================
tnPr            EQU       GRCMDS+16   ; 94h '_nPr_'
tnCr            EQU       GRCMDS+17   ; 95h '_nCr_'

;=========================================================================
;       more graph commands (start of graph commands requiring arguments)
;=========================================================================

tYOn            EQU       GRCMDS+18   ; 96h 'FnOn_'
tYOff           EQU       GRCMDS+19   ; 97h 'FnOff_'
tStPic          EQU       GRCMDS+20   ; 98h 'StPic_'
tRcPic          EQU       GRCMDS+21   ; 99h 'RcPic_'
tLine           EQU       GRCMDS+22   ; 9bh 'line'
tVert           EQU       GRCMDS+23   ; 9ch 'vert_'
tPtOn           EQU       GRCMDS+24   ; 9dh 'pton'
tPtOff          EQU       GRCMDS+25   ; 9eh 'ptoff'
;
;===============================================================
;  token a0 cannot be an eos function since low mult=a0 already
;===============================================================
tPtChg          EQU       GRCMDS+26   ; 9fh 'ptchg'
tPXOn           EQU       GRCMDS+27   ; a0h
tPXOff          EQU       GRCMDS+28   ; a1h
tPXChg          EQU       GRCMDS+29   ; a2h
tShade          EQU       GRCMDS+30   ; a3h 'shade'
tCircl          EQU       GRCMDS+31   ; a4h 'circl'
tHorz           EQU       GRCMDS+32   ; a5h 'horizontal'
tTanLn          EQU       GRCMDS+33   ; a6h 'tanln'
;tManFit         equ       GRCMDS+34   ; a7h 'manual fit'

;
;available      equ       GRCMDS+35   ; a8h
;
tDrInv          equ       GRCMDS+36   ; a9h 'drinv_'
tDrawF          equ       GRCMDS+36   ; a9h 'drawf_'
;
tVarStrng       equ       0aah
;
;===================================
; ===== functions with no arguments
;===================================
;
NOARG   EQU     0ABh
;
tRand           EQU       NOARG       ; ABh 'rand'
tPi             EQU       NOARG+1     ; ACh  Lpi
tGetKey         EQU       NOARG+2     ; ADh 'getKy'
;
;
tAPost          EQU       tGetKey+1   ; APOSTROPHY
tQuest          EQU       tAPost+1    ; QUESTION MARK
;
;
UNARY   EQU     tQuest+1              ; B0h
;
tChs            EQU       UNARY       ; B0h
tInt            EQU       UNARY+1     ; B1h
tAbs            EQU       UNARY+2     ; B2h
tDet            EQU       UNARY+3     ; B3h
tIdent          EQU       UNARY+4     ; B4h
tDim            EQU       UNARY+5     ; B5h
tSum            EQU       UNARY+6     ; B6h
tProd           EQU       UNARY+7     ; B7h
;tUnit            EQU       UNARY+8     ; B8h
tIPart          EQU       UNARY+9     ; B9h
tFPart          EQU       UNARY+10    ; BAh
;
;================================
;       new 2 byte tokens
;================================
t2ByteTok       equ       #0bb         ;
;
;
UNARYLR EQU     UNARY+12
;
tSqrt           EQU       UNARYLR     ; BCh
tCubRt          EQU       UNARYLR+1   ; BDh
tLn             EQU       UNARYLR+2   ; BEh
tExp            EQU       UNARYLR+3   ; BFh
tLog            EQU       UNARYLR+4   ; C0h
tALog           EQU       UNARYLR+5   ; C1h
tSin            EQU       UNARYLR+6   ; C2h
tASin           EQU       UNARYLR+7   ; C3h
tCos            EQU       UNARYLR+8   ; C4h
tACos           EQU       UNARYLR+9   ; C5h
tTan            EQU       UNARYLR+10  ; C6h
tATan           EQU       UNARYLR+11  ; C7h
LASTUNARY2       equ       UNARYLR+11  ; c7h
tMixSimp        equ     UNARYLR+12 ;c8convert between mixed(ab/c) & simple(b/c)
tFracDec        equ     UNARYLR+13 ;c9h convert between fraction & decimal.
tSimp           equ     UNARYLR+14 ;cah simplify a fraction.

;=================
;  binary op
;=================
tPower          equ       #0cb        ; cbh    '^'
tXRoot          equ       #0cc        ; cch    lsupx,lroot

;===================================
;       some programming commands
;===================================
;
PROGTOK equ     UNARYLR+17
;
tSetMenu        equ       PROGTOK     ; cdh
tIf             equ       PROGTOK+1   ; ceh
tThen           equ       PROGTOK+2   ; cfh
tElse           equ       PROGTOK+3   ; d0h
tWhile          equ       PROGTOK+4   ; d1h
tRepeat         equ       PROGTOK+5   ; d2h
tFor            equ       PROGTOK+6   ; d3h
tEnd            equ       PROGTOK+7   ; d4h
tReturn         equ       PROGTOK+8   ; d5h
tLbl            equ       PROGTOK+9   ; d6h   'lbl_'
tGoto           equ       PROGTOK+10  ; d7h   'goto_'
tPause          equ       PROGTOK+11  ; d8h   'pause_'
tStop           equ       PROGTOK+12  ; d9h   'stop'
tISG            equ       PROGTOK+13  ; dah   'is>'
tDSL            equ       PROGTOK+14  ; dbh   'ds<'
tInput          equ       PROGTOK+15  ; dch   'input_'
tPrompt         equ       PROGTOK+16  ; ddh   'prompt_'
tDisp           equ       PROGTOK+17  ; deh   'disp_'
tDispG          equ       PROGTOK+18  ; dfh   'dispg'
;
tOutput         equ       PROGTOK+19  ; e0h    'outpt'
tClLCD          equ       PROGTOK+20  ; e1h    'cllcd'
tFill           equ       PROGTOK+21  ; e2h    'fill'
tSortA          equ       PROGTOK+22  ; e3h    'sorta_'
tSortD          equ       PROGTOK+23  ; e4h    'sortd_'
tDispTab        equ       PROGTOK+24  ; e5h    'disp table
tMenu           equ       PROGTOK+25  ; e6h    'menu'
tSendMBL        equ       PROGTOK+26  ; e7h    'send'
tGetMBL         equ       PROGTOK+27  ; e8h    'get'
;
;===================================
;       stat plot commands
;===================================
statPCmd        equ       PROGTOK+28
;
tPlotOn         equ       statPCmd    ; e9h     ' plotson'
tPlotOff        equ       statPCmd+1   ; eah     ' plotsoff
CmdLen0         equ     tPlotOff - PROGTOK+ 1
CmdOff1         equ     CmdLen0 - 1
;
tListName       equ       #0eb        ; list designator
;
tPlot1          equ       #0ec
tPlot2          equ       #0ed
tPlot3          equ       #0ee

tUnit           equ       #0ef        ; efh unit for mixed fractions (a u b/c)
tFracSlash      equ       #0f0         ; f0h '/' (bc symbol)

tStatCmd         equ       #0f2
;
;==================================
; 2nd half of graph format tokens
;
; ===== format settings commands
;===================================

GFMT            equ       1
;
tSeq            equ       GFMT        ; 01  'seqg'
tSimulG         equ       GFMT+1      ; 02  'simulg'
tCoordOn        equ       GFMT+2      ; 03  'coordon'
tCoordOff       equ       GFMT+3      ; 04  'coordoff'
tDrawLine       equ       GFMT+4      ; 05  'drawline'
tDrawDot        equ       GFMT+5      ; 06  'drawdot'
tAxisOn         equ       GFMT+6      ; 07  'axeson'
tAxisOff        equ       GFMT+7      ; 08  'axesoff'
tGridOn         equ       GFMT+8      ; 09  'gridon'
tGridOff        equ       GFMT+9      ; 0a  'gridoff'
tLblOn          equ       GFMT+10     ; 0b  'labelon'
tLblOff         equ       GFMT+11     ; 0c  'labeloff'
tPolarG         equ       GFMT+12     ; 0d  'polargc'
tRectG          equ       GFMT+13     ; 0e  'rectgc'
;
;=====================================
; 2nd half of mode setting tokens
;=====================================
MODES2          equ       GFMT+14
;
tStndrd         equ       MODES2      ; 0fh 'func'
tParam          equ       MODES2+1    ; 10h 'param'
tPolar          equ       MODES2+2    ; 11h 'pol'
tAFillOn        equ       MODES2+3    ; 12h 'auto fill on
tAFillOff       equ       MODES2+4    ; 13h
tACalcOn        equ       MODES2+5    ; 14h
tACalcOff       equ       MODES2+6    ; 15h 'autofill off
;
tMABC           equ       MODES2+7    ; 16h 'aub/c mode'
tMBC            equ       MODES2+8    ; 17h 'b/c mode'
tMAutoSimp      equ       MODES2+9    ; 18h 'b/c mode'
tMManSimp       equ       MODES2+10   ; 19h 'b/c mode'
tMSingleC       equ       MODES2+11   ; 1ah single constant
tMMultiC        equ       MODES2+12   ; 1bh multi constant


STATREGS        equ     1
tOneVar         equ     STATREGS      ; 01h    'onevar_'
tTwoVar         equ     STATREGS+1    ; 02h
tLR1            equ     STATREGS+2    ; 03h    'linr(ax+b
tManFit         equ     STATREGS+5    ; 07h     'manual fit'
tMedMed         equ     STATREGS+6    ; 08h
tQuad           equ     STATREGS+7    ; 09h
tClrLst         equ     STATREGS+8    ; 0ah    'clear list
tClrTbl         equ     STATREGS+9    ; 0bh    clear table
tHist           equ     STATREGS+10   ; 0ch    'hist_'
tXYLine         equ     STATREGS+11   ; 0dh    'xyline_'
tScatter        equ     STATREGS+12   ; 0eh    'scatter_'
tLR             equ     STATREGS+13   ; 0fh    'linr(a+bx

;======================================
;       2nd half of user list tokens
;======================================
tL1             equ       #01         ; list 1
tL2             equ       #02         ; list 2
tL3             equ       #03         ; list 3
tL4             equ       #04         ; list 4
tL5             equ       #05         ; list 5
tL6             equ       #06         ; list 6

;===========================================
;       2nd half of user equation tokens
;===========================================
;
tY1             equ       #10         ; y1
tY2             equ       #11         ; y2
tY3             equ       #12         ; y3
tY4             equ       #13         ; y4
;
;       constant equations have bit 7 set
tC1             equ       #80         ; const1
tC2             equ       #81         ; const2
tC3             equ       #82         ; const3
tC4             equ       #83         ; const4
;
;===========================================
;       2nd half of user picture tokens
;===========================================
tPic1           equ       #01         ; pic1
tPic2           equ       #02         ; pic2
tPic3           equ       #03         ; pic3

;==============================================
;       2nd half of user graph database tokens
;==============================================
tGDB1           equ       #01         ; gdb1
tGDB2           equ       #02         ; gdb2
tGDB3           equ       #03         ; gdb3
tGDB4           equ       #04         ; gdb4
tGDB5           equ       #05         ; gdb5
tGDB6           equ       #06         ; gdb6
tGDB7           equ       #07         ; gdb7
tGDB8           equ       #08         ; gdb8
tGDB9           equ       #09         ; gdb9
tGDB0           equ       #0a         ; gdb0
;
;====================================
;       2nd half of string vars
;====================================
tStr1           equ       #01
tStr2           equ       #02
tStr3           equ       #03
tStr4           equ       #04
tStr5           equ       #05
tStr6           equ       #06
tStr7           equ       #07
tStr8           equ       #08
tStr9           equ       #09
tStr0           equ       #0a

;==================================================
;       2nd half of system output only variables
;==================================================
;
;   OPEN        EQU       #00
;
tRegEq          EQU       #01         ; REGRESSION EQUATION
;
tStatN          EQU       #02         ; STATISTICS N
;
tXMean          EQU       #03         ; X MEAN
tSumX           EQU       #04         ; SUM(X)
tSumXSqr        EQU       #05         ; SUM(X^2)
tStdX           EQU       #06         ; STANDARD DEV X
tStdPX          EQU       #07         ; STANDARD DEV POP X
tMinX           EQU       #08         ; Min X VALUE
tMaxX           EQU       #09         ; Max X VALUE
;
tMinY           EQU       #0A         ; Min Y VALUE
tMaxY           EQU       #0B         ; Max Y VALUE
tYmean          EQU       #0C         ; Y MEAN
tSumY           EQU       #0D         ; SUM(Y)
tSumYSqr        EQU       #0E         ; SUM(Y^2)
tStdY           EQU       #0F         ; STANDARD DEV Y
tStdPY          EQU       #10         ; STANDARD DEV POP Y
;
tSumXY          EQU       #11         ; SUM(XY)
tCorr           EQU       #12         ; CORRELATION
tMedX           EQU       #13         ; MED(X)
tQ1             EQU       #14         ; 1ST QUADRANT OF X
tQ3             EQU       #15         ; 3RD QUADRANT OF X
tQuadA          EQU       #16         ; 1ST TERM OF QUAD POLY REG/ Y-INT
tQuadB          EQU       #17         ; 2ND TERM OF QUAD POLY REG/ SLOPE
tQuadC          EQU       #18         ; 3RD TERM OF QUAD POLY REG
tCubeD          EQU       #19         ; 4TH TERM OF CUBIC POLY REG
tQuartE         EQU       #1A         ; 5TH TERM OF QUART POLY REG
tMedX1          EQU       #1B         ; x1 FOR MED-MED
tMedX2          EQU       #1C         ; x2 FOR MED-MED
tMedX3          EQU       #1D         ; x3 FOR MED-MED
tMedY1          EQU       #1E         ; y1 FOR MED-MED
tMedY2          EQU       #1F         ; y2 FOR MED-MED
tMedY3          EQU       #20         ; y3 FOR MED-MED
;
tLRSqr          equ       #21         ; r^2
tBRSqr          equ       #22         ; r^2
;
tError    equ     #23       ; token in case of error in entry (ans only)
tDone     equ     #24
;
;====================================
; system variable equates
;       vars with no restriction
;====================================
tuXscl       equ  1
tuYscl       equ  2
tXscl        equ  3
tYscl        equ  4

tXmin        EQU  #0A
tXmax        EQU  #0B
tYmin        EQU  #0C
tYmax        EQU  #0D
tTmin        EQU  #0E
tTmax        EQU  #0F
tThetaMin    EQU  #10
tThetaMax    EQU  #11
tuXmin       EQU  #12
tuXmax       EQU  #13
tuYmin       EQU  #14
tuYmax       EQU  #15
tuThetMin    EQU  #16
tuThetMax    EQU  #17
tuTmin       EQU  #18
tuTmax       EQU  #19
tTblMin      EQU  #1A
tPlotStart   EQU  #1B
tuPlotStart  EQU  #1C
tnMax        EQU  #1D
tunMax       EQU  #1E
tnMin        EQU  #1F
tunMin       EQU  #20

;
;       vars cannot be 0
;
tTblStep     equ  #21
tTStep       equ  #22
tThetaStep    equ  #23
tuTStep      equ  #24
tuThetStep     equ  #25
;
;       special limits on these
;
tDeltaX      equ  #26
tDeltaY      equ  #27
;
;       special limits on these
;
tXFact       equ  #28
tYFact       equ  #29
;
tTblInput       equ       #2a
;
;       no restrictions
tSimpFact       equ       #2b     ;simplification factor.
;
tPlotStep       equ       #34
tuPlotStep      equ       #35
;
tXres           equ       #36
tuXres          equ       #37
;

;======================================
;2nd byte of 2byte tokens
;======================================

tLCM            equ       #01         ; immediate unary
tGCD            equ       #02         ; immediate unary
tRandInt        equ       #03         ; immediate unary
tSubStrng       equ       #04         ; immediate unary
tStdDev         equ       #05         ; immediate unary
tVariance       equ       #06         ; immediate unary
tInStrng        equ       #07         ; immediate unary
tCoin           equ       #08         ; immediate unary
tDice           equ       #09         ; immediate unary
tRemainder      equ       #0a         ; immediate unary
lastImUnary     equ       #0a         ; last immediate unary token.
firstUnary      equ       #0b         ; first unary operator.
tCumSum         equ       #0b         ; unary
tExpr           equ       #0c         ; unary                        do autop
tLength         equ       #0d         ; unary                        do autop
tDeltaLst       equ       #0e         ; unary                        do autop
tRef            equ       #0f         ; unary                        no autop
autoP10         equ       #10
tRRef           equ       #10         ; unary                        no autop
newTok12        equ       #12

autoP11         equ       #12
tMatToLst       equ       #12         ; command pushed               do autop
tLstToMat       equ       #13         ; command pushed               do autop
tSetConst       equ       #14         ;                                 autop
tGraphStyle     equ       #15         ; command pushed                  autop
autoP12         equ       #15
tSetUpLst       equ       #16         ; command opt pushed           no autop
tExprOn         equ       #1a         ; mode command
tExprOff        equ       #1b         ; mode command
tClrSllLst      equ       #1c         ; command not pushed
tGetCalc        equ       #1d         ; command not pushed
tDelVar         equ       #1e         ; command not pushed
tEquToStrng     equ       #1f         ; command not pushed
tStrngToEqu     equ       #20         ; command not pushed
tDelLast        equ       #21         ; command not pushed
tSelect         equ       #22         ; command not pushed
tModBox         equ       #23         ; icon
tNormProb       equ       #24         ; icon
tPictOlot       equ       #25
tCirclePlot     equ       #26
tStemPlot       equ       #27
tBarPlot        equ       #28
tZFit           equ       #29         ; zoom fit
tDiag_On        equ       #2a         ; dianostic display on
tDiag_Off       equ       #2b         ; dianostic display off

;==========================================
;	data type equates
;===========================================
RealObj         equ       0
ListObj         equ       1
MatObj          equ       2
EquObj          equ       3
StrngObj        equ       4
ProgObj         equ       5
ProtProgObj     equ       6
PictObj         equ       7
GDBObj          equ       8
UnknownObj      equ       9
UnknownEquObj   equ       #0a
NewEquObj       equ       #0b
CplxObj         equ       #0c
ClistObj        equ       #0d
SlistObj        equ       #0d   ;string list
UndefObj        equ       #0e
WindowObj       equ       #0f
ZStoObj         equ       #10
TblrngObj       equ       #11
LCDObj          equ       #12
BackupObj       equ       #13
FracObj         equ       #14
FracRSObj       equ       #14           ;reduced   simple fraction.
FracRMObj       equ       #15           ;reduced   mixed  fraction.
FracUSObj       equ       #16           ;unreduced simple fraction.
FracUMObj       equ       #17           ;unreduced mixed fraction.
NameObj         equ       #18           ;name string (will fit in fplen).
AppObj          equ       #19           ;application, only used in menus/link
AppVarObj       equ       #1a           ;application variable
TmpConstObj     equ       #1b           ;temporary type of constant.

;======================================================================
;           i/o equates                                                          
;======================================================================
;
D0D1_bits	EQU  #03
D0LD1L		EQU  #03
D0LD1H		EQU  #01
D0HD1L		EQU  #02
D0HD1H		EQU  #00
bport		EQU  0			; 4-BIT LINK PORT (I/O) 
;
;======================================================================
;           device codes                                                       
;======================================================================
;
;
TI82DEV		EQU  #82
PC82DEV		EQU  #02
MAC82DEV	EQU  #12
;
LINK83FDEV	EQU  #23
;
TI83DEV		EQU  #83
PC83DEV		EQU  #03
MAC83DEV	EQU  #13
;
TI85DEV		EQU  #95      ; DIFF THEN REAL 85 SO ME TALK
PC85DEV		EQU  #05
MAC85DEV	EQU  #15
;
TI73DEV		EQU  #74      ; Device x3 is always an 83
PC73DEV		EQU  #07
MAC73DEV	EQU  #17
;
TI83FDEV	EQU  #73
LINK73FDEV	EQU  #23
PC83FDEV	EQU  #23

;=========================================
;	system error codes
;=========================================

E_EDITF		EQU	7		;allow re-entering application
E_EDIT		EQU	1<<E_EDITF
E_Mask		EQU	7Fh

E_Overflow	EQU	1+E_EDIT
E_DivBy0	EQU	2+E_EDIT
E_SingularMat	EQU	3+E_EDIT
E_Domain	EQU	4+E_EDIT
E_Increment	EQU	5+E_EDIT
E_Break		EQU	6+E_EDIT
E_NonReal       EQU     7+E_EDIT
E_Syntax        EQU     8+E_EDIT
E_DataType      EQU     9+E_EDIT
E_Argument      EQU     10+E_EDIT
E_DimMismatch   EQU     11+E_EDIT
E_Dimension     EQU     12+E_EDIT
E_Undefined     EQU     13+E_EDIT
E_Memory        EQU     14+E_EDIT
E_Invalid       EQU     15+E_EDIT
E_IllegalNest   EQU     16+E_EDIT
E_Bound         EQU     17+E_EDIT
E_GraphRange    EQU     18+E_EDIT
E_Zoom          EQU     19+E_EDIT
E_Label         EQU     20
E_Stat          EQU     21
E_Solver        EQU     22+E_EDIT
E_Singularity   EQU     23+E_EDIT
E_SignChange    EQU     24+E_EDIT
E_Iterations    EQU     25+E_EDIT
E_BadGuess      EQU     26+E_EDIT
E_StatPlot      EQU     27
E_TolTooSmall   EQU     28+E_EDIT
E_Reserved      EQU     29+E_EDIT
E_Mode          EQU     30+E_EDIT
;
E_Scale         equ     31+E_EDIT
;
E_LnkErr        equ     32+E_EDIT
E_LnkMemErr     equ     33+E_EDIT
E_LnkTransErr   equ     34+E_EDIT
E_LnkDupErr     equ     35+E_EDIT
E_LnkMemFull    equ     36+E_EDIT
E_Unknown       equ     37+E_EDIT
E_IdNotFound    equ     38+E_EDIT
E_NoMode        equ     39+E_EDIT
E_Validation    equ     40+E_EDIT
E_Length        equ     41+E_EDIT
E_Application   equ     42+E_EDIT
E_AppErr1       equ     43+E_EDIT
E_AppErr2       equ     44+E_EDIT
E_ExpiredApp	equ	45+E_EDIT
E_BadAddr	equ	46+E_EDIT
HigErrNum	equ	46+E_EDIT

; obsolete error numbers34              ;first link error
E_LinkIOChkSum	equ	34
E_LinkIOTimeOut	equ	35
E_LinkIOBusy	equ	36
E_LinkIOVer	equ	37

;==============================================
;       equates to ram locations for stat vars
;==============================================
;
StatN   EQU     statVars
XMean   EQU     StatN      + FPLEN
SumX    EQU     XMean      + FPLEN
SumXSqr EQU     SumX       + FPLEN
StdX    EQU     SumXSqr    + FPLEN
StdPX   EQU     StdX       + FPLEN
MinX    EQU     StdPX      + FPLEN
MaxX    EQU     MinX       + FPLEN
MinY    EQU     MaxX       + FPLEN
MaxY    EQU     MinY       + FPLEN
YMean   EQU     MaxY       + FPLEN
SumY    EQU     YMean      + FPLEN
SumYSqr EQU     SumY       + FPLEN
StdY    EQU     SumYSqr    + FPLEN
StdPY   EQU     StdY       + FPLEN
SumXY   EQU     StdPY      + FPLEN
Corr    EQU     SumXY      + FPLEN
MedX    EQU     Corr       + FPLEN
Q1      EQU     MedX       + FPLEN
Q3      EQU     Q1         + FPLEN
QuadA   EQU     Q3         + FPLEN
QuadB   EQU     QuadA      + FPLEN
QuadC   EQU     QuadB      + FPLEN
CubeD   EQU     QuadC      + FPLEN
QuartE  EQU     CubeD      + FPLEN
MedX1   EQU     QuartE     + FPLEN
MedX2   EQU     MedX1      + FPLEN
MedX3   EQU     MedX2      + FPLEN
MedY1   EQU     MedX3      + FPLEN
MedY2   EQU     MedY1      + FPLEN
MedY3   EQU     MedY2      + FPLEN
SLower  EQU     MedY3      + 2*FPLEN
SUpper  EQU     MedY3      + FPLEN

L3rd        equ              OP6+4
R3rd        equ              OP6+6
RidX        equ              OP6
LidX        equ              OP6+2

;Text below by Michael Vincent.

;SAFERAM
saferam1 equ appBackUpScreen	;768 bytes free for use
;saferam2 equ savesscreen		;768 bytes - APD mem
;saferam3 equ tempswaparea	;~232 bytes or so.
saferam4 equ OP1
;saferam5 equ imathptr1		

;SHELL EQUATES
Version equ statVars
Random equ statVars+3
PutSprite equ statVars+6
LargeSprite equ statVars+9
GetPixel equ statVars+12
Decompress equ statVars+15
FastCopy equ statVars+18
LevelDetect equ statVars+21

;Equates to allow 83+ ION programs to be easily recompiled for Mallard.
ionVersion equ Version
ionRandom equ Random
ionPutSprite equ PutSprite
ionLargeSprite equ LargeSprite
ionGetPixel equ GetPixel
ionDecompress equ Decompress
ionFastCopy equ FastCopy
ionDetect equ LevelDetect

