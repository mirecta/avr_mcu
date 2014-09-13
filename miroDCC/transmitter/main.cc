#include <util/delay.h>
#include <avr/io.h>



void delay_ms(int d){
    while (d--){
        _delay_ms(1);
    }
}



int main(){

    DDRB |= ((1 << PB5) | (1 << PB4));

    PORTB &= ~(1 << PB5);

    while(true){
    PORTB |= 1 << PB5;
    delay_ms(700);
    PORTB &= ~(1 << PB5);
    delay_ms(300);

    }
}
