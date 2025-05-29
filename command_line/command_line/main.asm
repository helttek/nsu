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

.def TMP = r16
.def DEVICE_NUM = R17
.def FUNC_TYPE = R18
.def MEM_ADDR_LOW = R19
.def MEM_ADDR_HIGH = R20
.def DATA_VALUE = R21
.def CMD_INDEX = R22

.dseg
CMD_BUFFER: .byte 10

.CSEG
RESET:
    ; init stack
    ldi r16, LOW(RAMEND)
    out SPL, r16
    ldi r16, HIGH(RAMEND)
    out SPH, r16

    ldi r17, 0x00
    ldi r16, 0x67
    call USART_Init

loop:
    jmp loop

USART_Init:
    ; set baud rate
    sts UBRR0H, r17
    sts UBRR0L, r16

    ; enable receiver and transmitter
    ldi r16, (1<<RXEN0)|(1<<TXEN0)
    sts UCSR0B, r16

    ; set format
    ldi r16, (0<<USBS0)|(3<<UCSZ00)
    sts UCSR0C, r16

    ; enable interrupts
    lds r16, UCSR0B
    ori r16, (1<<RXCIE0)
    sts UCSR0B, r16
    sei

    ret

USART_RXC:
    lds TMP, UDR0

	; check CR
    cpi TMP, 13
    breq PROCESS_COMMAND

    ; save cmd to buf
    ldi ZH, HIGH(CMD_BUFFER)
    ldi ZL, LOW(CMD_BUFFER)
    add ZL, CMD_INDEX
    st Z, TMP

    inc CMD_INDEX

    rjmp END_RXC

PROCESS_COMMAND:
	ldi ZH, HIGH(CMD_BUFFER)
    ldi ZL, LOW(CMD_BUFFER)
    clr CMD_INDEX
    
    ldd r16, Z+1
    cpi r16, 'R'
    breq PREPARE_READ_RESPONSE
	cpi r16, 'W'
	breq EXECUTE_WRITE
    rjmp RESET_BUFFER

PREPARE_READ_RESPONSE:
    ; device number
    ldd r16, Z+0 
    call USART_SEND_BYTE

    ; operation type
    ldd r16, Z+1 
    call USART_SEND_BYTE

    ; low addr
    ldd r16, Z+2 
	mov YL, r16
    call USART_SEND_BYTE

    ; high addr
    ldd r16, Z+3
	mov YH, r16
    call USART_SEND_BYTE

	; send data
	ldi r16, ':'
    call USART_SEND_BYTE
	ldi r16, ' '
    call USART_SEND_BYTE
    ld r16, Y
    call USART_SEND_BYTE
    ldi r16, '\n'
    call USART_SEND_BYTE
    rjmp RESET_BUFFER

EXECUTE_WRITE:
    ldd MEM_ADDR_LOW, Z+2
    ldd MEM_ADDR_HIGH, Z+3

    mov YL, MEM_ADDR_LOW
    mov YH, MEM_ADDR_HIGH

    ; write data to sram
	ldd r16, Z+4
    st Y, r16
	rjmp RESET_BUFFER

RESET_BUFFER:
    clr CMD_INDEX

    ldi ZH, HIGH(CMD_BUFFER)
    ldi ZL, LOW(CMD_BUFFER)
    clr r16
END_RXC:
    reti

USART_SEND_BYTE:
wait_tx_ready:
    lds r17, UCSR0A
    sbrs r17, UDRE0
    rjmp wait_tx_ready

    sts UDR0, r16
    ret