.include "m168def.inc"

.def temp = r16
.def adc_l = r17
.def adc_h = r18
.def hundreds = r19
.def tens = r20
.def ones = r21

.org 0x0000
    rjmp RESET

RESET:
    ldi temp, HIGH(RAMEND)
    out SPH, temp
    ldi temp, LOW(RAMEND)
    out SPL, temp

    rcall UART_Init
    rcall ADC_Init
    
MAIN:
    rcall ADC_Start

	rcall Send_header

	mov temp, adc_h
	rcall ConvertBCD
	rcall Send_Result

	mov temp, adc_l
    rcall ConvertBCD
    rcall Send_Result
    
	rcall Send_CR

    rjmp MAIN

UART_Init:
    ; Set baud rate
 	sts UBRR0H, r17
 	sts UBRR0L, r16
 	; Enable receiver and transmitter
 	ldi r16, (1<<TXEN0)
 	sts UCSR0B, r16
 	; 8 data bits, 1 stop bit
 	ldi r16, (0<<USBS0)|(3<<UCSZ00)
 	sts UCSR0C, r16
    
    ret

ADC_Init:
    ; setup voltage reference
    ldi temp, (1<<REFS1)|(1<<REFS0)
    sts ADMUX, temp
    
    ; enable ADC, set division factor
    ldi temp, (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)
    sts ADCSRA, temp
    
    ret

ADC_Start:
	; start conversion
    lds temp, ADCSRA
    ori temp, (1<<ADSC)
    sts ADCSRA, temp

ADC_Wait:
	; wait for the conversion to finish
    lds temp, ADCSRA
    sbrc temp, ADSC
    rjmp ADC_Wait

ADC_Read:
	; get results
    lds adc_l, ADCL
    lds adc_h, ADCH
    
    ret

ConvertBCD:
    clr hundreds
    clr tens

HundredsLoop:
    cpi     temp, 100
    brlt    TensLoopStart
    subi    temp, 100
    inc     hundreds
    rjmp    HundredsLoop

TensLoopStart:
TensLoop:
    cpi     temp, 10
    brlt    UnitsSetup
    subi    temp, 10
    inc     tens
    rjmp    TensLoop

UnitsSetup:
    mov     ones, temp

    ret

Send_Result:
    mov temp, hundreds
    subi temp, -'0'
    rcall UART_Transmit
    
    mov temp, tens
    subi temp, -'0'
    rcall UART_Transmit
    
    mov temp, ones
    subi temp, -'0'
    rcall UART_Transmit
    
    ret

Send_header:
	ldi temp, 'A'
    rcall UART_Transmit
    ldi temp, 'D'
    rcall UART_Transmit
    ldi temp, 'C'
    rcall UART_Transmit
    ldi temp, ' '
    rcall UART_Transmit
    ldi temp, 'V'
    rcall UART_Transmit
    ldi temp, 'a'
    rcall UART_Transmit
    ldi temp, 'l'
    rcall UART_Transmit
    ldi temp, 'u'
    rcall UART_Transmit
    ldi temp, 'e'
    rcall UART_Transmit
    ldi temp, ':'
    rcall UART_Transmit
    ldi temp, ' '
    rcall UART_Transmit

	ret

Send_CR:
	ldi temp, 0x0D ; CR
    rcall UART_Transmit
    ldi temp, 0x0A ; LF
    rcall UART_Transmit

	ret

UART_Transmit:
    lds r30, UCSR0A
    sbrs r30, UDRE0
    rjmp UART_Transmit

    sts UDR0, temp
    
    ret
