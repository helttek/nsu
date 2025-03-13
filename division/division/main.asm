;
; division.asm
;
; Created: 13.03.2025 13:38:35
; Author : justa
;

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

; Replace with your application code
RESET:
    ldi	r16, LOW(RAMEND)
	out	SPL, r16
	ldi	r16, HIGH(RAMEND)
	out	SPH, r16

