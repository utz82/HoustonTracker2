;collapsed savestate

	db #10			;speed
	;dw #5fcd		;usr drum

IF MODEL = TI82
	dw #5e31
ENDIF

IF MODEL = TI83
	dw #2bae
ENDIF

IF MODEL = TI8X
	dw #1f73
ENDIF
	db #01			;loop point

	db #00,#02,#01,#00	;ptn sequence
	db #00,#02,#01,#01
	db #ff

				;ptn area
	db 24			;regular note byte
	db #d6			;#d6 -> d: 0-byte, 6: for the next 6+1 = 7 rows
 	db 24,24,24,48
 	db 43,43,43,43
	
	db #d1,24,#d5
	db 24,24,36
	db 41,41,41,41
	
	db 0,24,#d5
	db 24,0,24,24
	db 12,24,36,48
	
	db #ff			;end of ptn area
	

				;fx ptn area
	db #01+#80		;fx ptn# (#00). bit 7 set = last fx ptn.
		
	db #1f,#20
	db #20,0
	db #30,0
	db #20,0
	
	db #ff,#10
	db #f0,0
	db #f0,0
	db #f0,0
	
	db #90,0
	db #a0,0
	db 0,0
	db #c0,0
	
	db #d0,0
	db #e0,0
	db #f0,0
	db 0,0

;eof