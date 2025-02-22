;
; interrupt_handler.asm
;
; Created: 19.02.2025 3:56:05
; Author : justa
;


; Replace with your application code

.include "m168def.inc"

.CSEG

.def tmp= r16

; initialize interrupt handlers
.org $0000 rjmp RESET
.org $0002 rjmp EXT_INT0
.org $0004 rjmp EXT_INT1
.org $0006 rjmp PCINT0_handler
.org $0008 rjmp PCINT1_handler
.org $000A rjmp PCINT2_handler
.org $000C rjmp WDT
.org $000E rjmp TIM2_COMPA
.org $0010 rjmp TIM2_COMPB
.org $0012 rjmp TIM2_OVF
.org $0014 rjmp TIM1_CAPT
.org $0016 rjmp TIM1_COMPA
.org $0018 rjmp TIM1_COMPB
.org $001A rjmp TIM1_OVF
.org $001C rjmp TIM0_COMPA
.org $001E rjmp TIM0_COMPB
.org $0020 rjmp TIM0_OVF
.org $0022 rjmp SPI_STC
.org $0024 rjmp USART_RXC
.org $0026 rjmp USART_UDRE
.org $0028 rjmp USART_TXC
.org $002A rjmp ADC_handler
.org $002C rjmp EE_RDY
.org $002E rjmp ANA_COMP
.org $0030 rjmp TWI
.org $0032 rjmp SPM_RDY

RESET:
	; initialize stack pointer
    ldi	r16, LOW(RAMEND)
	out	SPL, r16
	ldi	r16, HIGH(RAMEND)
	out	SPH, r16

	; enable interrupts
	sei 	

// tcra tcrb tcnt timsk
// interrupt vector table p.51
// set up interrupt vector at timer overflow
//
// обработать прерывание от переполнения таймера

EXT_INT0:

EXT_INT1:

PCINT0_handler:

PCINT1_handler:

PCINT2_handler:

WDT:

TIM2_COMPA:

TIM2_COMPB:

TIM2_OVF:

TIM1_CAPT:

TIM1_COMPA:

TIM1_COMPB:

TIM1_OVF:

TIM0_COMPA:

TIM0_COMPB:

TIM0_OVF:

SPI_STC:

USART_RXC:

USART_UDRE:

USART_TXC:

ADC_handler:

EE_RDY:

ANA_COMP:

TWI:

SPM_RDY: