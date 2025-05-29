.include "m168def.inc"

.equ F_CPU = 16000000

.def tmp = r16
.def spi_data = r17
.def counter = r18

.equ SPI_PORT = PORTB
.equ SPI_DDR = DDRB
.equ SCK = PB5
.equ SS = PB2
.equ MOSI = PB3
.equ MISO = PB4

.org 0x0000 rjmp start
.org INT_VECTORS_SIZE

start:
    ldi r16, high(RAMEND)
    out SPH, r16
    ldi r16, low(RAMEND)
    out SPL, r16

    rcall spi_init

    clr counter

main_loop:
    cbi SPI_PORT, SS

    mov spi_data, counter
    rcall spi_transmit

    sbi SPI_PORT, SS

    inc counter

    rcall delay

    rjmp main_loop

spi_init:
    sbi SPI_DDR, MOSI
    sbi SPI_DDR, SCK
    sbi SPI_DDR, SS
    cbi SPI_DDR, MISO

    sbi SPI_PORT, SS

    ldi tmp, (1<<SPE) | (1<<MSTR) | (1<<SPR0)
    out SPCR, tmp

    ret

spi_transmit:
	push tmp
	in tmp, SREG
	push tmp

    out SPDR, spi_data
spi_wait:
    in tmp, SPSR
    sbrs tmp, SPIF
    rjmp spi_wait

	pop tmp
	out SREG, tmp
	pop tmp
    ret

delay:
    push r23
    push r24
    push r25
    ldi r23, 110
delay_loop_1:
    ldi r24, 100
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
