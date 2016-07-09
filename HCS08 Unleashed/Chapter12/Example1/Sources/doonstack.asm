;**************************************************************
;* This stationery is meant to serve as the framework for a   *
;* user application. For a more comprehensive program that    *
;* demonstrates the more advanced functionality of this       *
;* processor, please see the demonstration applications       *
;* located in the examples subdirectory of the                *
;* Metrowerks Codewarrior for the HC08 Program directory      *
;**************************************************************
; export symbols
            XDEF DoOnStack
            XDEF FlashErase
            XDEF FlashProg
            ; we use export 'Entry' as symbol. This allows us to
            ; reference 'Entry' either in the linker .prm file
            ; or from C/C++ later on
                       
; include derivative specific macros
            Include 'MC9S08GB60.inc'
            
;mPageErase equ $40
;mByteProg equ $20
mFACCERR equ $10
mFPVIOL equ $20
mFCBEF equ $80

            

; variable/data section
MY_ZEROPAGE: SECTION  SHORT
; Insert here your data definition. For demonstration, temp_byte is used.
; temp_byte ds.b 1

; code section
MyCode:     SECTION
;**************************************************************
; this assembly routine is called the C/C++ application
DoOnStack: 	pshx
			pshh ;save pointer to flash
			psha ;save command on stack
			ldhx #SpSubEnd ;point at last byte to move to stack;
SpMoveLoop: lda ,x ;read from flash
			psha ;move onto stack
			aix #-1 ;next byte to move
			cphx #SpSub-1 ;past end?
			bne SpMoveLoop ;loop till whole sub on stack
			tsx ;point to sub on stack
			tpa ;move CCR to A for testing
			and #$08 ;check the I mask
			bne I_set ;skip if I already set
			sei ;block interrupts while FLASH busy
			lda SpSubSize+6,sp ;preload data for command
			jsr ,x ;execute the sub on the stack
			cli ;ok to clear I mask now
			bra I_cont ;continue to stack de-allocation
I_set: 		lda SpSubSize+6,sp ;preload data for command
			jsr ,x ;execute the sub on the stack
I_cont: 	ais #SpSubSize+3 ;deallocate sub body + H:X + command ;H:X flash pointer OK from SpSub
			lsla ;A=00 & Z=1 unless PVIOL or ACCERR
			rts ;to flash where DoOnStack was called		
;**************************************************************
SpSub: 		ldhx LOW(SpSubSize+4),sp ;get flash address from stack
			sta 0,x ;write to flash; latch addr and data
			lda SpSubSize+3,sp ;get flash command
			sta FCMD ;write the flash command
			lda #mFCBEF ;mask to initiate command
			sta FSTAT ;[pwpp] register command
			nop ;[p] want min 4~ from w cycle to r
ChkDone: 	lda FSTAT ;[prpp] so FCCF is valid
			lsla ;FCCF now in MSB
			bpl ChkDone ;loop if FCCF = 0
SpSubEnd: 	rts ;back into DoOnStack in flash
SpSubSize: 	equ (*-SpSub)
;**************************************************************
FlashErase: psha ;adjust sp for DoOnStack entry
			lda #(mFPVIOL+mFACCERR) ;mask
			sta FSTAT ;abort any command and clear errors
			lda #mPageErase ;mask pattern for page erase command
			bsr DoOnStack ;finish command from stack-based sub
			ais #1 ;deallocate data location from stack
			rts
;**************************************************************
FlashProg:  psha ;temporarily save entry data
			lda #(mFPVIOL+mFACCERR) ;mask
			sta FSTAT ;abort any command and clear errors
			lda #mByteProg ;mask pattern for byte prog command
			bsr DoOnStack ;execute prog code from stack RAM
			ais #1 ;deallocate data location from stack
			rts
;**************************************************************