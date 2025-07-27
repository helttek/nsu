.include "m168def.inc"

.macro LOAD_Z
    ldi r30, low(@0)
    ldi r31, high(@0)
.endmacro

.def tmp = r16
.def key_value = r17
.def segment_pattern = r18
.def col_mask = r19
.def row_idx = r20
.def col_idx = r21
.def row_mask = r22

.equ SEGMENT_0 = 0b00111111
.equ SEGMENT_1 = 0b00000110
.equ SEGMENT_2 = 0b01011011
.equ SEGMENT_3 = 0b01001111
.equ SEGMENT_4 = 0b01100110
.equ SEGMENT_5 = 0b01101101
.equ SEGMENT_6 = 0b01111101
.equ SEGMENT_7 = 0b00000111
.equ SEGMENT_8 = 0b01111111
.equ SEGMENT_9 = 0b01101111
.equ SEGMENT_A = 0b01110111
.equ SEGMENT_C = 0b00111001
.equ SEGMENT_BLANK = 0b0

.org 0x0000 rjmp start

start:
	ldi tmp, low(RAMEND)
    out SPL, tmp
    ldi tmp, high(RAMEND)
    out SPH, tmp

    ldi tmp, 0xFF
    out DDRB, tmp
	ldi tmp, SEGMENT_BLANK
    out PORTB, tmp

	; rows
	ldi tmp, 0x00
	out DDRC, tmp
	ldi tmp, 0x0F
	out PORTC, tmp

    ; columns
    ldi tmp, 0x00
    out DDRD, tmp
	ldi tmp, 0x07
    out PORTD, tmp

    ldi segment_pattern, SEGMENT_BLANK
    out PORTB, segment_pattern

main_loop:
	rcall scan_keypad
	cpi key_value, 0xFF
	brne pressed_key

	ldi segment_pattern, SEGMENT_BLANK
	out PORTB, segment_pattern
	rjmp after_update

pressed_key:
	rcall keypad_to_segment
	out PORTB, segment_pattern

after_update:
	rcall delay
	rjmp main_loop

scan_keypad:
    ldi key_value, 0xFF
    ldi row_idx, 0
    ldi row_mask, 0b00000001

row_loop:
    mov tmp, row_mask
	; output
    out DDRC, tmp 
    com tmp
    andi tmp, 0x0F
    out PORTC, tmp

    nop
    nop

    in col_mask, PIND
    andi col_mask, 0x07

    cpi col_mask, 0x07
    breq next_row

	com col_mask
	andi col_mask, 0x07

	ldi col_idx, 0
find_col:
	lsr col_mask
	; carry
	brcs got_col

	inc col_idx
	cpi col_idx, 3
	brlt find_col

	rjmp scan_done

got_col:
	mov key_value, row_idx
	ldi tmp, 3
	mul key_value, tmp
	add r0, col_idx
	mov key_value, r0
	clr r1
	rjmp scan_done

next_row:
	lsl row_mask
	inc row_idx
	cpi row_idx, 4
	brne row_loop

scan_done:
	ldi tmp, 0x00
	out DDRC, tmp
	ldi tmp, 0x0F
	out PORTC, tmp
	ret

keypad_to_segment:
    cpi key_value, 0xFF
    breq DISPLAY_BLANK
    cpi key_value, 12
    brsh DISPLAY_BLANK

    cpi key_value, 0
    breq DISPLAY_1
    cpi key_value, 1
    breq DISPLAY_2
    cpi key_value, 2
    breq DISPLAY_3
    cpi key_value, 3
    breq DISPLAY_4
    cpi key_value, 4
    breq DISPLAY_5
    cpi key_value, 5
    breq DISPLAY_6
    cpi key_value, 6
    breq DISPLAY_7
    cpi key_value, 7
    breq DISPLAY_8
    cpi key_value, 8
    breq DISPLAY_9
	cpi key_value, 9
	breq DISPLAY_A
	cpi key_value, 10
	breq DISPLAY_0
DISPLAY_C:
	ldi segment_pattern, SEGMENT_C
	ret
DISPLAY_0:
    ldi segment_pattern, SEGMENT_0
    ret
DISPLAY_1:
    ldi segment_pattern, SEGMENT_1
    ret
DISPLAY_2:
    ldi segment_pattern, SEGMENT_2
    ret
DISPLAY_3:
    ldi segment_pattern, SEGMENT_3
    ret
DISPLAY_4:
    ldi segment_pattern, SEGMENT_4
    ret
DISPLAY_5:
    ldi segment_pattern, SEGMENT_5
    ret
DISPLAY_6:
    ldi segment_pattern, SEGMENT_6
    ret
DISPLAY_7:
    ldi segment_pattern, SEGMENT_7
    ret
DISPLAY_8:
    ldi segment_pattern, SEGMENT_8
    ret
DISPLAY_9:
    ldi segment_pattern, SEGMENT_9
    ret
DISPLAY_A:
	ldi segment_pattern, SEGMENT_A
	ret
DISPLAY_BLANK:
    ldi segment_pattern, SEGMENT_BLANK
    ret

delay:
    push r23
    push r24
    push r25
    ldi r23, 20
delay_loop_1:
    ldi r24, 20
delay_loop_2:
    ldi r25, 20
delay_loop_3:
    dec r25
    brne delay_loop_3
    dec r24
    brne delay_loop_2
    dec r23
    brne delay_loop_1
	pop r23
    pop r24
    pop r25
    ret
