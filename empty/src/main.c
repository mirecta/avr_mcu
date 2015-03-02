
#include <avr/io.h>
#include <util/delay.h>


void delayms(uint16_t millis) {
    while ( millis ) {
        _delay_ms(1);
        --millis;
    }
}


void main(){


    DDRD |= 1<<PD7; /* set PD7 to output */
    while(1) {
        PORTD &= ~(1<<PD7); /* LED on */
        delayms(100);
        PORTD |= 1<<PD7; /* LED off */
        delayms(900);
    }

}
