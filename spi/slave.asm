.include "m168def.inc"

.equ F_CPU = 16000000
.equ BAUD = 9600
.equ UBRR_VAL = F_CPU / 16 / BAUD - 1

.def tmp = r16
.def spi_recv_data = r17

.equ SPI_PORT = PORTB
.equ SPI_DDR = DDRB
.equ SCK = PB5
.equ SS = PB2
.equ MOSI = PB3
.equ MISO = PB4

.org 0x0000 rjmp start 
.org 0x0022 rjmp SPI_INT

start:
    ldi     tmp, high(RAMEND)
    out     SPH, tmp
    ldi     tmp, low(RAMEND)
    out     SPL, tmp

    rcall   spi_init

    rcall   uart_init

    sei

main_loop:
    rjmp    main_loop

spi_init:
    sbi SPI_DDR, MISO
    cbi SPI_DDR, MOSI
    cbi SPI_DDR, SCK
    cbi SPI_DDR, SS

    ldi tmp, (1<<SPE) | (1<<SPIE)
    out SPCR, tmp

    ret

uart_init:
    ldi tmp, high(UBRR_VAL)
    sts UBRR0H, tmp
    ldi tmp, low(UBRR_VAL)
    sts UBRR0L, tmp

    ldi tmp, (1<<TXEN0)
    sts UCSR0B, tmp

    ldi tmp, (1<<UCSZ01) | (1<<UCSZ00)
    sts UCSR0C, tmp

    ret

SPI_INT:
    push tmp
    in tmp, SREG
    push tmp

    in tmp, SPDR
    rcall uart_transmit

    pop tmp
    out SREG, tmp
    pop tmp

    reti

uart_transmit:
    push spi_recv_data
    in spi_recv_data, SREG
    push spi_recv_data

tx_wait:
    lds spi_recv_data, UCSR0A
    sbrs spi_recv_data, UDRE0
    rjmp tx_wait

    sts UDR0, tmp

    pop spi_recv_data
    out SREG, spi_recv_data
    pop spi_recv_data
    ret
