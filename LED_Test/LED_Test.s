#define __SFR_OFFSET 0x00
#include "avr/io.h"
;------------------------
.global thing
.global work

thing:
pop:MOV     R30, R24
    LDI     R31, 0
    RET

work:
    OUT     PORTD, R30
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

    OUT     PORTD, R31
    NOP
    NOP
    NOP
    NOP
    JMP     pop
