#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//58us --- 1
//100us ----0

volatile uint8_t counter = 0;


void delay_ms(int d){
    while (d--){
        _delay_ms(1);
    }
}


ISR(TIMER0_OVF_vect ){

if (counter == 10){
    counter = 0;
    if(PORTB & (1 << PB7))
        PORTB &= ~(1 << PB7);
    else
        PORTB |= 1 << PB7;
}
 ++counter;

}


int main(){

    DDRB |= ((1 << PB7) | (1 << PB4));

    PORTB &= ~(1 << PB7);



    TIMSK0 |=  (1 << TOIE0);

    TCCR0B = (1 << CS02) | (1 << CS00);

    sei();


    while(true){
   /* PORTB |= 1 << PB5;
    delay_ms(700);
    PORTB &= ~(1 << PB5);
    delay_ms(300);*/

    }
}
