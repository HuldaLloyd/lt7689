;/**************************************************************************//**
; * @file     startup_ARMCM4.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM4 Device Series
; * @version  V1.08
; * @date     23. November 2012
; *
; * @note
; *
; ******************************************************************************/
;/* Copyright (c) 2011 - 2012 ARM LIMITED
;
;   All rights reserved.
;   Redistribution and use in source and binary forms, with or without
;   modification, are permitted provided that the following conditions are met:
;   - Redistributions of source code must retain the above copyright
;     notice, this list of conditions and the following disclaimer.
;   - Redistributions in binary form must reproduce the above copyright
;     notice, this list of conditions and the following disclaimer in the
;     documentation and/or other materials provided with the distribution.
;   - Neither the name of ARM nor the names of its contributors may be used
;     to endorse or promote products derived from this software without
;     specific prior written permission.
;   *
;   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
;   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;   POSSIBILITY OF SUCH DAMAGE.
;   ---------------------------------------------------------------------------*/
;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00005000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00004000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     EFM_IRQHandler            ;  0:  EFM
                DCD     PMU_IRQHandler            ;  1:  PMU
                DCD     TC_IRQHandler             ;  2:  TC
                DCD     PIT1_IRQHandler           ;  3:  PIT1
                DCD     PIT2_IRQHandler           ;  4:  PIT2
                DCD     ENCR_IRQHandler           ;  5:  ENCR
                DCD     ENCR1_IRQHandler          ;  6:  ENCR1
                DCD     DMA1_IRQHandler            ;  7:  DMA
                DCD     DMA2_IRQHandler           ;  8:  DMA2
                DCD     DMA2D_IRQHandler          ;  9:  DMA2D
                DCD     TRNG_IRQHandler           ; 10:  TRNG
                DCD     SEC_PGD_LD_FD_IRQHandler  ; 11:  SEC_DET/PGD/LD/FD Combined Interrupt
                DCD     PCI_DET_IRQHandler        ; 12:  PCI
                DCD     ASYNC_TIMER_IRQHandler    ; 13:  ASYNC timer
				DCD     PCI_IRQHandler            ; 14:  PCI
                DCD     PMU_RTC_IRQHandler        ; 15:  PMU RTC
                DCD     RSA_IRQHandler            ; 16:  RSA
                DCD     SHA_IRQHandler            ; 17:  SHA
                DCD     AES_IRQHandler            ; 18:  AES
                DCD     SMS4_IRQHandler           ; 19:  SMS4
                DCD     QADC_IRQHandler           ; 20:  QADC
                DCD     DAC_IRQHandler            ; 21:  DAC
                DCD     MCC_IRQHandler            ; 22:  MCC
                DCD     TSI_IRQHandler            ; 23:  TSI
                DCD     USBC_IRQHandler           ; 24:  USBC
                DCD     MIPI_IRQHandler           ; 25:  MIPI
                DCD     SPI1_IRQHandler           ; 26:  SPI1
                DCD     SPI2_IRQHandler           ; 27:  SPI2
                DCD     SPI3_IRQHandler           ; 28:  SPI3
                DCD     SPIM1_IRQHandler          ; 29:  SPIM1
                DCD     SPIM2_IRQHandler          ; 30:  SPIM2
                DCD     SPIM3_IRQHandler          ; 31:  SPIM3
				DCD     SCI1_IRQHandler           ; 32:  SCI1
                DCD     SCI2_IRQHandler           ; 33:  SCI2
                DCD     USI2_IRQHandler           ; 34:  USI2
                DCD     RESERVED1          		  ; 35:  RESERVED
				DCD     I2C1_IRQHandler           ; 36:  I2C1
				DCD     PWM0_IRQHandler           ; 37: PWM0
        		DCD     PWM1_IRQHandler           ; 38: PWM1
                DCD     PWM2_IRQHandler           ; 39: PWM2
                DCD     PWM3_IRQHandler           ; 40: PWM3
				DCD     EPORT0_0_IRQHandler	      ; 41: EPORT0_0
                DCD     EPORT0_1_IRQHandler	      ; 42: EPORT0_1
                DCD     EPORT0_2_IRQHandler       ; 43: EPORT0_2
                DCD     EPORT0_3_IRQHandler       ; 44: EPORT0_3
                DCD     EPORT0_4_IRQHandler       ; 45: EPORT0_4
                DCD     EPORT0_5_IRQHandler       ; 46: EPORT0_5
                DCD     EPORT0_6_IRQHandler       ; 47: EPORT0_6
                DCD     EPORT0_7_IRQHandler       ; 48: EPORT0_7
				DCD     EPORT1_0_IRQHandler	      ; 49: EPORT1_0
                DCD     EPORT1_1_IRQHandler	      ; 50: EPORT1_1
                DCD     EPORT1_2_IRQHandler       ; 51: EPORT1_2
                DCD     EPORT1_3_IRQHandler       ; 52: EPORT1_3
                DCD     EPORT1_4_IRQHandler       ; 53: EPORT1_4
                DCD     EPORT1_5_IRQHandler       ; 54: EPORT1_5
                DCD     EPORT1_6_IRQHandler       ; 55: EPORT1_6
                DCD     EPORT1_7_IRQHandler	      ; 56: EPORT1_7
				DCD     CLCD_IRQHandler           ; 57: CLCD
				DCD     DCMI_IRQHandler			  ; 58: DCMI	
                DCD     I2C2_IRQHandler           ; 59:  I2C2
                DCD     I2C3_IRQHandler           ; 60:  I2C3
                DCD     SCI3_IRQHandler           ; 61:  SCI3
                DCD     RESERVED2		          ; 62:  reserved
				DCD     USI1_IRQHandler           ; 63:  USI1
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler
				
Reset_Handler   PROC				
                EXPORT  Reset_Handler             [WEAK]
				
				;enable LVD
				;CPM_PWRCR |= 0x0c;
				;MOV		r1,#0x40004000
				;LDR		r0,[r1,#0x30]
				;ORR		r0,r0,#0x0c
				;STR		r0,[r1,#0x30]
				
				IMPORT  LVD_EN
				LDR     R0, =LVD_EN
                BLX     R0
					
;set lvd level at 2.0V 
				MOV		r0,#0x40000000
				MOV		r1,#0x40004000
				STR		r0,[r1,#0x5c]
				LSLS	r0,r0,#1
				STR		r0,[r1,#0x5c]
				ASRS	r0,r0,#1
				STR		r0,[r1,#0x5c]
				MOV		r0,r1
				LDR		r0,[r0,#0x5c]
				ORR		r0,r0,#0x400000
				STR		r0,[r1,#0x5c]
				
				LDR		r0,=0x81001000
				MOV		r1,#0x40004000
				STR		r0,[r1,#0x54]
				
				MOV		r1,#0x40004000
				LDR		r0,[r1,#0x5c]
				BIC		r0,r0,#0xc0000000
				STR		r0,[r1,#0x5c]
;set lvd level at 2.0V end

				;clear i/o latch for por from poff1.0/poff1.5
				;CPM_PWRCR = (CPM_PWRCR &~ 0x40000000) | 0x80000000;
				;CPM_PWRCR &=~ 0x80000000;
				;MOV		r1,#0x40004000
				;LDR		r0,[r1,#0x30]
				;BIC		r0,r0,#0x40000000
				;ORR		r0,r0,#0x80000000
				;STR		r0,[r1,#0x30]
				;BIC		r0,r0,#0x80000000
				;STR		r0,[r1,#0x30]
				
				IMPORT  IO_Latch_Clr
				LDR     R0, =IO_Latch_Clr
                BLX     R0
				

                IMPORT  SystemInit
                IMPORT  __main
				MOV     r0,#0xe
                LDR     R1,=0x40005000
				;STRH    r0,[r1,#0]       ;Close WDT
				MOV     r0,#0
				LDR     r1,=0x40006000
				STRH    r0,[r1,#0]       ;Close TC	
				
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  EFM_IRQHandler            [WEAK]
                EXPORT  PMU_IRQHandler            [WEAK]
                EXPORT  TC_IRQHandler             [WEAK]
                EXPORT  PIT1_IRQHandler           [WEAK]
                EXPORT  PIT2_IRQHandler           [WEAK]
                EXPORT  ENCR_IRQHandler           [WEAK]
                EXPORT  ENCR1_IRQHandler          [WEAK]
                EXPORT  DMA1_IRQHandler           [WEAK]
                EXPORT  DMA2_IRQHandler           [WEAK]
                EXPORT  DMA2D_IRQHandler          [WEAK]
                EXPORT  TRNG_IRQHandler           [WEAK]
                EXPORT  SEC_PGD_LD_FD_IRQHandler  [WEAK]
                EXPORT  PCI_DET_IRQHandler        [WEAK]
                EXPORT  ASYNC_TIMER_IRQHandler    [WEAK]
                EXPORT  PCI_IRQHandler            [WEAK]
                EXPORT  PMU_RTC_IRQHandler        [WEAK]
                EXPORT  RSA_IRQHandler            [WEAK]
                EXPORT  SHA_IRQHandler            [WEAK]
                EXPORT  AES_IRQHandler            [WEAK]
                EXPORT  SMS4_IRQHandler           [WEAK]
                EXPORT  QADC_IRQHandler           [WEAK]
                EXPORT  DAC_IRQHandler            [WEAK]
                EXPORT  MCC_IRQHandler            [WEAK]
                EXPORT  TSI_IRQHandler            [WEAK]					
                EXPORT  USBC_IRQHandler           [WEAK]					
                EXPORT  MIPI_IRQHandler           [WEAK]
                EXPORT  SPI1_IRQHandler           [WEAK]					
                EXPORT  SPI2_IRQHandler           [WEAK]					
                EXPORT  SPI3_IRQHandler           [WEAK]					
                EXPORT  SPIM1_IRQHandler          [WEAK]					
                EXPORT  SPIM2_IRQHandler          [WEAK]
                EXPORT  SPIM3_IRQHandler          [WEAK]
                EXPORT  SCI1_IRQHandler           [WEAK]
                EXPORT  SCI2_IRQHandler           [WEAK]
                EXPORT  USI2_IRQHandler           [WEAK]					
                EXPORT  RESERVED1                 [WEAK]
                EXPORT  I2C1_IRQHandler           [WEAK]					
                EXPORT  PWM0_IRQHandler           [WEAK]					
                EXPORT  PWM1_IRQHandler           [WEAK]					
                EXPORT  PWM2_IRQHandler           [WEAK]					
                EXPORT  PWM3_IRQHandler           [WEAK]
                EXPORT  EPORT0_0_IRQHandler       [WEAK]
                EXPORT  EPORT0_1_IRQHandler       [WEAK]
                EXPORT  EPORT0_2_IRQHandler       [WEAK]
                EXPORT  EPORT0_3_IRQHandler       [WEAK]
                EXPORT  EPORT0_4_IRQHandler       [WEAK]
                EXPORT  EPORT0_5_IRQHandler       [WEAK]
                EXPORT  EPORT0_6_IRQHandler       [WEAK]
                EXPORT  EPORT0_7_IRQHandler       [WEAK]
                EXPORT  EPORT1_0_IRQHandler       [WEAK]
                EXPORT  EPORT1_1_IRQHandler       [WEAK]
                EXPORT  EPORT1_2_IRQHandler       [WEAK]
                EXPORT  EPORT1_3_IRQHandler       [WEAK]
                EXPORT  EPORT1_4_IRQHandler       [WEAK]
                EXPORT  EPORT1_5_IRQHandler       [WEAK]
                EXPORT  EPORT1_6_IRQHandler       [WEAK]
                EXPORT  EPORT1_7_IRQHandler       [WEAK]
                EXPORT  CLCD_IRQHandler           [WEAK]
                EXPORT  DCMI_IRQHandler           [WEAK]
				EXPORT  I2C2_IRQHandler           [WEAK]	
				EXPORT  I2C3_IRQHandler           [WEAK]	
				EXPORT  SCI3_IRQHandler           [WEAK]	
		        EXPORT  RESERVED2		          [WEAK]
				EXPORT  USI1_IRQHandler           [WEAK]

EFM_IRQHandler
PMU_IRQHandler
TC_IRQHandler
PIT1_IRQHandler
PIT2_IRQHandler
ENCR_IRQHandler
ENCR1_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA2D_IRQHandler
TRNG_IRQHandler
SEC_PGD_LD_FD_IRQHandler
PCI_DET_IRQHandler
ASYNC_TIMER_IRQHandler
PCI_IRQHandler
PMU_RTC_IRQHandler
RSA_IRQHandler
SHA_IRQHandler
AES_IRQHandler
SMS4_IRQHandler
QADC_IRQHandler
DAC_IRQHandler
MCC_IRQHandler
TSI_IRQHandler
USBC_IRQHandler
MIPI_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
SPI3_IRQHandler
SPIM1_IRQHandler
SPIM2_IRQHandler
SPIM3_IRQHandler
SCI1_IRQHandler
SCI2_IRQHandler
USI2_IRQHandler
RESERVED1
I2C1_IRQHandler
PWM0_IRQHandler
PWM1_IRQHandler
PWM2_IRQHandler
PWM3_IRQHandler
EPORT0_0_IRQHandler
EPORT0_1_IRQHandler
EPORT0_2_IRQHandler
EPORT0_3_IRQHandler
EPORT0_4_IRQHandler
EPORT0_5_IRQHandler
EPORT0_6_IRQHandler
EPORT0_7_IRQHandler
EPORT1_0_IRQHandler
EPORT1_1_IRQHandler
EPORT1_2_IRQHandler
EPORT1_3_IRQHandler
EPORT1_4_IRQHandler
EPORT1_5_IRQHandler
EPORT1_6_IRQHandler
EPORT1_7_IRQHandler
CLCD_IRQHandler
DCMI_IRQHandler
I2C2_IRQHandler
I2C3_IRQHandler
SCI3_IRQHandler
RESERVED2
USI1_IRQHandler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
