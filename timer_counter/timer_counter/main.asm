.include "m168def.inc"

.def temp = r16
.def counter = r17
.def led_state = r18
.def delay_multiplier = r19

.org 0x0000 rjmp RESET
.org 0x0020 rjmp TIMER1_OVERFLOW

RESET:
    ldi temp, HIGH(RAMEND)
    out SPH, temp
    ldi temp, LOW(RAMEND)
    out SPL, temp

    ldi temp, (1<<PB5)
    out DDRB, temp

    ldi led_state, 0
    ldi delay_multiplier, 2
    
	; set normal mode
    ldi temp, 0
    sts TCCR1A, temp
    
	; enable overflow interrupt
    ldi temp, (1<<TOIE1)
    sts TIMSK1, temp

	; 2-byte_MAX - 49911 = 15625 clk - number of ticks at 16MHz for 1 second to pass with 1024 prescaler+
    ldi temp, HIGH(49911)
    sts TCNT1H, temp
    ldi temp, LOW(49911)
    sts TCNT1L, temp
    
    ; set prescaler (1024)
    ldi temp, (1<<CS12) | (1<<CS10)
    sts TCCR1B, temp
    
    sei

MAIN_LOOP:
    rjmp MAIN_LOOP

TIMER1_OVERFLOW:
    ldi temp, HIGH(49911)
    sts TCNT1H, temp
    ldi temp, LOW(49911)
    sts TCNT1L, temp
    
    dec delay_multiplier
    brne END_INTERRUPT
    
    com led_state
    andi led_state, 1
    cpi led_state, 1
    brne LED_OFF
    
LED_ON:
    sbi PORTB, PB5
    ldi delay_multiplier, 1
    rjmp END_INTERRUPT
    
LED_OFF:
    cbi PORTB, PB5
    ldi delay_multiplier, 2
    
END_INTERRUPT:
    reti