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
.org $0020 reti
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
    ldi	r16, LOW(RAMEND)
	out	SPL, r16
	ldi	r16, HIGH(RAMEND)
	out	SPH, r16

    lds    r16, PRR
    andi   r16, ~(1 << PRTIM0)
    sts    PRR, r16 ; power reduction register

	; set OC0A as output -- to see the PWM
	ldi   r16, (1 << PD6)
	out   DDRD, r16 ; data direction register

	ldi   r16, (1 << COM0A1) | (1 << WGM01) | (1 << WGM00) ; WGM00, WGM01 - set in fast pwm mode, COM0A1 - set non inverted mode
	out   TCCR0A, r16

	; set 0 prescaling
	ldi   r16, (1 << CS00)
	out   TCCR0B, r16

	; set 50% duty cycle
	ldi   r16, 127
	out   OCR0A, r16

loop:
	rjmp loop            ; Infinite loop to keep the MCU busy