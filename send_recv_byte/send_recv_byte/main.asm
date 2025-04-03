;
; send_recv_byte.asm
;
; Created: 26.03.2025 14:48:40
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

	ldi r17, 0x00
	ldi r16, 0x67 ; 103 in hex
	rcall USART_Init

	ldi r16, 'A'
	rcall USART_Transmit

	rjmp loop

loop:
	rcall USART_Receive
	rcall USART_Transmit
	rjmp loop

USART_Init:
	; The Power Reduction USART bit, PRUSART0, in “Power Reduction Register - PRR” on page 39 must be disabled by writing a logical zero to it.
	;lds    r16, PRR
    ;andi   r16, ~(1 << PRUSART0)
    ;sts    PRR, r16
	; Set baud rate
	sts UBRR0H, r17
	sts UBRR0L, r16
	; Enable receiver and transmitter
	ldi r16, (1<<RXEN0)|(1<<TXEN0)
	sts UCSR0B, r16
	; 8 data bits, 1 stop bit
	ldi r16, (0<<USBS0)|(3<<UCSZ00)
	sts UCSR0C, r16
	ret

USART_Transmit:
 ; Wait for empty transmit buffer
 lds r18, UCSR0A
 sbrs r18, UDRE0
 rjmp USART_Transmit
 ; Put data (r16) into buffer, sends the data
 sts UDR0, r16
 ret

USART_Receive:
 ; Wait for data to be received
 lds r18, UCSR0A
 sbrs r18, RXC0
 rjmp USART_Receive
 ; Get and return received data from buffer
 lds r16, UDR0
 ret

