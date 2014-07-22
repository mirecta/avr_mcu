#include <util/delay.h>
#include <stdarg.h>
#include <stdio.h> 

#include "matrix.h"
#include "dht22.h"




LedMatrix_t lm;
DHT22 dht(PC2, PORTC, DDRC, PINC);

/*

#define FONT_4x6     1
#define FONT_5x7     2
#define FONT_5x8     3
#define FONT_8x8    16

*/


void print(int x, int y, char* fmt, ...){
    char buf[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);
    lm.printText(x,y,buf,1);
}


int main(){



    lm.clear();
    lm.brightness(8);
    lm.setFont(FONT_5x8);
    int i = 0;

    while(1){

    dht.read_data();

    if (i <= 25){
    int t = dht.get_temperature_c_int();
    
    lm.clearFb();
    print(0,0, " %02d",t/10);
    lm.putChar(13,0, '.');
    print(18,0, "%1dC",t%10);
    lm.updateFb();
    }
    else{
    int h = dht.get_humidity_int();
    
    lm.clearFb();
    lm.printText(0,0,"Rh",1);
    print(12,0, "%02d",h/10);
    lm.putChar(20,0, '.');
    print(25,0, "%1d",h%10);
    lm.updateFb();
    
    
    }


    if (i >= 35)
        i = 0;

    
    
    delay_ms(200);
    i++;

}


    return 0;

}
