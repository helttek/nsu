 ;
 ; 10-byte_string_send_recv.asm
 ;
 ; Created: 09.04.2025 18:14:28
 ; Author : justa
 ;

 .include "m168def.inc"

 .CSEG

 .org $0000 jmp RESET 
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
 .org $0024 jmp USART_RXC
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

 	ldi r17, 0x00
 	ldi r16, 0x67
 	call USART_Init

 	jmp loop

 loop:
 	jmp loop

 USART_Init:
 	; Set baud rate
 	sts UBRR0H, r17
 	sts UBRR0L, r16
 	; Enable receiver and transmitter
 	ldi r16, (1<<RXEN0)|(1<<TXEN0)
 	sts UCSR0B, r16
 	; 8 data bits, 1 stop bit
 	ldi r16, (0<<USBS0)|(3<<UCSZ00)
 	sts UCSR0C, r16

 	
 	; enable recieve coimplete interrupt
 	lds r16, UCSR0B
 	ori r16, (1<<RXCIE0)
 	sts UCSR0B, r16
	sei

 	; init starting address
 	ldi r17, 0 ; low addr in sram
 	ldi r18, 1 ; high addr in sram
 	ldi r20, 0 ; index
 	ret

 USART_RXC:
 	lds r16, UDR0

 	mov r30, r17
 	mov r31, r18
 	st Z, r16

 	cpi r16, 13
 	breq send_string

 	cpi r20, 9
 	breq send_string

 	inc r17 ; addr
 	inc r20 ; index

 	reti

 send_string:
 	clr r20
 	ldi r17, 0

 	mov r30, r17
 	mov r31, r18

 	jmp send_loop
 	reti

send_loop:
	wait_tx_ready:
    lds r16, UCSR0A
	sbrs r16, UDRE0 ; skip if buffer empty
	rjmp wait_tx_ready

 	ld r16, Z+

 	sts UDR0, r16

 	cpi r16, 13 ; if end of string is met
 	breq end_send_loop

	cpi r20, 9
 	breq end_send_loop

 	inc r20
 	jmp send_loop

 end_send_loop:
 	clr r17
 	clr r20
 	reti
