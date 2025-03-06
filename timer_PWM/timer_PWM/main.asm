;
; timer_PWM.asm
;
; Created: 06.03.2025 13:16:17
; Author : justa
;

; Таймер-счетчик в режиме генерации ШИМ-сигнала. Настроить конфигурационные регистры для 
; работы таймера-счетчика в режиме генерации ШИМ-сигнала (Коэффициент заполнения 50%).

; The PRTIM0 bit in “Power Reduction Register - PRR” on page 39 must be written to zero to enable Timer/Counter0 module.

; Replace with your application code
.include "m168def.inc"

.CSEG

.def tmp = r16

; initialize interrupt handlers
.org $0000 rjmp RESET 
.org $0002 reti
.org $0004 reti
.org $0006 reti
.org $0008 reti
.org $000A reti
.org $000C reti
.org $000E reti
.org $0010 reti
.org $0012 reti
.org $0014 reti
.org $0016 reti
.org $0018 reti
.org $001A reti
.org $001C reti
.org $001E reti
.org $0020 rjmp TIM0_OVF
.org $0022 reti
.org $0024 reti
.org $0026 reti
.org $0028 reti
.org $002A reti
.org $002C reti
.org $002E reti
.org $0030 reti
.org $0032 reti

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

	sbi DDRB, PB3
	
	; CTC mode
	ldi  r16, (1 << WGM01)
    out TCCR0A, r16 

	; Toggle OC0A on compare match
    ldi  r16, (1 << COM0A0)
    sts  TCCR0A, r16

    ; Set OCR0A (Compare Match Value)
    ldi  r16, 249   ; (16MHz / (64 * (249 + 1))) = 1 kHz
    sts  OCR0A, r16

    ; Set Timer0 prescaler to 64
    ldi  r16, 3; (1 << CS01) | (1 << CS00)
    sts  TCCR0B, r16

loop:
	rjmp loop

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
