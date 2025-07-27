;
; find_element_in_array.asm
;
; Created: 06.02.2025 10:37:08
; Author : justa
;


; Replace with your application code

.include "m168def.inc"

.DSEG 
	array: .BYTE 5
	result: .BYTE 1

.CSEG

.def element = r16
.def i = r17
.def tmp = r18
.def res = r19

.org $000 rjmp start

start:
	; init stack pointer
	ldi	r16, LOW(RAMEND)
	out	SPL, r16
	ldi	r16, HIGH(RAMEND)
	out	SPH, r16

	; put the arguments to Z (array) and r16 (element to find) registers
	ldi ZL, low(array) 
	ldi ZH, high(array) 
	ldi r16, 23

	rcall init_array

	rcall find_elem_in_array

; expects array to be loaded to Z
init_array:
	push ZL
	push ZH
	push i
	push tmp
w
	ldi i, 0
	ldi tmp, 0

init_loop:
	cpi i, 5 // i < 5
	breq break_init_loop// branch when equal
	inc i // i++

	; init with indexes
	inc tmp
	st Z+, tmp // post increment the address, after storing register in memory location

	rjmp init_loop

break_init_loop:
	pop tmp
	pop i
	pop ZH
	pop ZL
	ret

; result will be stored in register r19
find_elem_in_array:
	push i
	push ZL
	push ZH
	push tmp

	ldi res, -1
	clr i
	ldi ZL, low(array) // get lower part of the address
	ldi ZH, high(array) // get the higher part of the address
	rjmp fea_main

fea_main:
	cpi i, 5 // i < 5
	breq fea_break_loop2 // branch when equal
	inc i // i++

	ld tmp, Z+ // post increment the address, after loading the contents to the register
	cp tmp, element
	breq fea_break_loop
	
	rjmp fea_main

fea_break_loop2:
	pop tmp
	pop ZH
	pop ZL
	pop i
	ret

fea_break_loop:
	dec i
	mov res, i
	pop tmp
	pop ZH
	pop ZL
	pop i
	ret
