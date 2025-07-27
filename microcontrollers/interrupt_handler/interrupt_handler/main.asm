;
; interrupt_handler.asm
;
; Created: 19.02.2025 3:56:05
; Author : justa
;


; Replace with your application code

.include "m168def.inc"

.CSEG

.def tmp = r16

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

	; The PRTIM0 bit in “Power Reduction Register - PRR” on page 39 must be written to zero to enable Timer/Counter0 module.
	lds   r16, PRR
    andi  r16, ~(1 << PRTIM0)
    sts PRR, r16

	; enable interrupts
	sei

	; setup Timer/Counter0 in normal mode
	; WGM01:0 = 0
	ldi   r16, 0
	out TCCR0A, r16

	; setup a prescaler of 64
	ldi   r16, 1; 3 - for 64 prescaler
	out   TCCR0B, r16 

	; enable Timer/Counter0 overflow interrupt
	ldi   r16, (1<<TOIE0)
	sts TIMSK0, r16

	; set result register to 0 to make sure the interrupt handler is working
	ldi r17, 0

	rjmp loop


loop:
	rjmp loop


// interrupt vector table p.58
//
// The PRTIM0 bit in “Power Reduction Register - PRR” on page 39 must be written to zero to enable Timer/Counter0 module.
//
// Прерывания. Осуществить настройку модуля таймера-счетчика микроконтроллера в базовом режиме счета с произвольным делителем.
// Настроить вектор прерывания «Переполнение таймера-счетчика». Написать алгоритм обработки прерывания «Переполнения таймера-счетчика».

EXT_INT0:
	reti

EXT_INT1:
	reti

PCINT0_handler:
	reti

PCINT1_handler:
	reti

PCINT2_handler:
	reti

WDT:
	reti

TIM2_COMPA:
	reti

TIM2_COMPB:
	reti

TIM2_OVF:
	reti

TIM1_CAPT:
	reti

TIM1_COMPA:
	reti

TIM1_COMPB:
	reti

TIM1_OVF:
	reti

TIM0_COMPA:
	reti

TIM0_COMPB:
	reti

TIM0_OVF:
	; save status register
	push r16
	in r16, SREG
	push r16

	ldi r17, 0
	out TCCR0B, r17 ; stop the timer (clear clock source, prescaler)

	ldi r17, 1 ; indicating that the interrupt handler has been visited

	pop r16
	out SREG, r16
	pop r16
	reti

SPI_STC:
	reti

USART_RXC:
	reti

USART_UDRE:
	reti

USART_TXC:
	reti

ADC_handler:
	reti

EE_RDY:
	reti

ANA_COMP:
	reti

TWI:
	reti

SPM_RDY:
	reti