#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#ifndef LED_MATRIX_H 

#define LED_MATRIX_H
#define FONT_4x6     1
#define FONT_5x7     2
#define FONT_5x8     3
#define FONT_8x8    16

#include "font.h"

#define STR_CONCAT(a,b)   a ## b
#define D_OUTPORT(name)   STR_CONCAT(PORT,name)
#define D_DDRPORT(name)   STR_CONCAT(DDR,name)


#define HT1632_ID_CMD        4  /* ID = 100 - Commands */
#define HT1632_ID_RD         6  /* ID = 110 - Read RAM */
#define HT1632_ID_WR         5  /* ID = 101 - Write RAM */

#define HT1632_CMD_SYSDIS 0x00  /* CMD= 0000-0000-x Turn off oscil */
#define HT1632_CMD_SYSON  0x01  /* CMD= 0000-0001-x Enable system oscil */
#define HT1632_CMD_LEDOFF 0x02  /* CMD= 0000-0010-x LED duty cycle gen off */
#define HT1632_CMD_LEDON  0x03  /* CMD= 0000-0011-x LEDs ON */
#define HT1632_CMD_BLOFF  0x08  /* CMD= 0000-1000-x Blink ON */
#define HT1632_CMD_BLON   0x09  /* CMD= 0000-1001-x Blink Off */
#define HT1632_CMD_SLVMD  0x10  /* CMD= 0001-00xx-x Slave Mode */
#define HT1632_CMD_MSTMD  0x14  /* CMD= 0001-01xx-x Master Mode */
#define HT1632_CMD_RCCLK  0x18  /* CMD= 0001-10xx-x Use on-chip clock */
#define HT1632_CMD_EXTCLK 0x1C  /* CMD= 0001-11xx-x Use external clock */
#define HT1632_CMD_COMS00 0x20  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS01 0x24  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS10 0x28  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_COMS11 0x2C  /* CMD= 0010-ABxx-x commons options */
#define HT1632_CMD_PWM    0xA0  /* CMD= 101x-PPPP-x PWM duty cycle */

#define HT1632_ID_LEN     (1 << 2)  /* IDs are 3 bits */
#define HT1632_CMD_LEN    (1 << 7)  /* CMDs are 8 bits */
#define HT1632_DATA_LEN   (1 << 7)  /* Data are 4*2 bits */
#define HT1632_ADDR_LEN   (1 << 6)  /* Address are 7 bits */


#define HT1632_DDR DDRB
#define HT1632_PORT PORTB
#define HT1632_WR 4
#define HT1632_CS 3
#define HT1632_DATA 5

#define X_MAX 31
#define Y_MAX 7

void delay_ms(int d){
     while (d--){ 
       _delay_ms(1);
    }
}

class LedMatrix_t{


    public:
        LedMatrix_t(){
            HT1632_DDR  |= (1 << HT1632_WR) | (1 << HT1632_CS) | (1 << HT1632_DATA);
            
            disable();

            HT1632_PORT |= 1 << HT1632_WR | 1 << HT1632_DATA;

            setup();
        
        
        }

        void setup(){

            sendCMD(HT1632_CMD_SYSDIS);
            sendCMD(HT1632_CMD_COMS00);
            sendCMD(HT1632_CMD_MSTMD);
            sendCMD(HT1632_CMD_RCCLK);
            sendCMD(HT1632_CMD_SYSON);
            sendCMD(HT1632_CMD_LEDON);
            sendCMD(HT1632_CMD_PWM);
        }

        void clearFb(){
            memset(fb,0,32);
        }
       
        inline void putPixel(int8_t x, int8_t y, char color){

            int addr;
            int offset;

            if(x < 0 || x > X_MAX) return;
            if(y < 0 || y > Y_MAX) return;

              
             // 10 , 3
            addr = (y) + (x & 0xf8); 
           

            offset = 7  - ((x) & 0x07);

            if (color)
             fb[addr] |= 1 << offset;
            else
             fb[addr] &= ~(1 << offset);
        }
       


        void hScrollText(int y, char *text, int delaytime, int times, uint8_t dir, uint8_t color = 1, uint8_t space = 0)
        {
            int i, x, offs = 0, len = strlen(text);
            uint8_t width = font_width -1 + space;
            width++;
            while (times) {
                for ((dir) ? x = - (len * width) : x = X_MAX; (dir) ? x <= X_MAX : x > - (len * width); (dir) ? x++ : x--)
                {
                    clearFb();            
                    for (i = 0; i < len; i++)
                    {
                        (dir) ? offs-- : offs++;
                        offs = (width ) * i;
                        putChar(x + offs,  y, text[i], color);
                    }
/*                    for (i = 0; i < len; i++)
                    {
                        offs = width * i;
                        putChar(x + offs,  y, text[i]);
                    }*/
                    sendFrame();
                    delay_ms(delaytime);
                }
                times--;
            }
        }
 
        void vScrollText(int x, char *text, int delaytime, int times, uint8_t dir, uint8_t color = 1  ,uint8_t space = 0)
        {
            int i, y, voffs, len = strlen(text);
            uint8_t offs;
            uint8_t width = font_width;
            width++;

            while (times) {
                for ((dir) ? y = - font_height : y = Y_MAX + 1; (dir) ? y <= Y_MAX + 1 : y > - font_height; (dir) ? y++ : y--)
                {
                    clearFb();
                    for (i = 0; i < len; i++)
                    {
                        offs = ((width - 1) + space) * i;
                        voffs = (dir) ? -1 : 1;                       
                        putChar(x + offs,  y + voffs, text[i], color);
                    }
                    sendFrame();
                    delay_ms(delaytime);
                }
                times--;
            }
        }
        
        void  printText(int x,int y, char *text, uint8_t space = 0, uint8_t color = 1){

            uint8_t offs = 0;
            int len = strlen(text);

            for(int i = 0; i < len; i++){
                putChar(x + offs,y,text[i],color);
                offs += font_width - 1 + space;
             } 
        }      
 
        uint8_t putChar(int x, int y, char c, uint8_t color = 1)
        {
            uint16_t dots, msb;
            char col, row;

            uint8_t width = font_width;
            uint8_t height = font_height;

            //y += font_height;
            if ( x + font_width < 0 || x > X_MAX || y + font_height < 0 || y >=Y_MAX )
                  return 0;


            if ((unsigned char) c >= 0xc0) c -= 0x41;
            c -= 0x20;
            msb = 1 << (height - 1);

            // some math with pointers... don't try this at home ;-)
            uint8_t* PROGMEM addr = font + c * width;

            for (col = 0; col < width; col++) {
                dots = pgm_read_byte_near(addr + col);
                for (row = 0; row < height; row++) {
                    if (dots & (msb >> row)) {
                        putPixel(x + col, (y + row), color);
                    } else {
                        //putPixel(x + col, (y + row), 0);
                    }
                }
            }
            return ++width;
        }

        


        void setFont(uint8_t userfont){

            switch(userfont) {

                #ifdef FONT_4x6
                case FONT_4x6:
                    font = (uint8_t*) &font_4x6[0];
                    font_width = 4;
                    font_height = 6;
                    break;
                #endif
                #ifdef FONT_5x7
                case FONT_5x7:
                    font = (uint8_t*) &font_5x7[0];
                    font_width = 5;
                    font_height = 7;
                    break;
                #endif
                #ifdef FONT_5x8
                case FONT_5x8:
                    font = (uint8_t*) &font_5x8[0];
                    font_width = 5;
                    font_height = 8;
                    break;
                #endif
                #ifdef FONT_5x7W
                case FONT_5x7W:
                    font = (uint8_t*) &font_5x7w[0];
                    font_width = 5;
                    font_height = 7;
                    break;
               #endif

               #ifdef FONT_8x8
               case FONT_8x8:
                   font = (uint8_t*) &font_8x8[0];
                   font_width = 8;
                   font_height = 8;
                   break;
              #endif


            }
        }
        
        void line(int x0, int y0, int x1, int y1, uint8_t color = 1)
        {
            int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
            int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
            int err = dx + dy, e2; /* error value e_xy */

            for(;;) {
                putPixel(x0, y0, color);
                if (x0 == x1 && y0 == y1) break;
                e2 = 2 * err;
                if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
                if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
            }
        }

        void rect(int x0, int y0, int x1, int y1, uint8_t color = 1)
        {
            line(x0, y0, x0, y1, color); /* left line   */
            line(x1, y0, x1, y1, color); /* right line  */
            line(x0, y0, x1, y0, color); /* top line    */
            line(x0, y1, x1, y1, color); /* bottom line */
        }

        void circle(int xm, int ym, int r, uint8_t color = 1)
        {
            int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
            do {
                putPixel(xm - x, ym + y, color); /*   I. Quadrant */
                putPixel(xm - y, ym - x, color); /*  II. Quadrant */
                putPixel(xm + x, ym - y, color); /* III. Quadrant */
                putPixel(xm + y, ym + x, color); /*  IV. Quadrant */
                r = err;
                if (r >  x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
                if (r <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
            } while (x < 0);
        }


        void clear(){
            clearFb();
            sendFrame();
        }

        void updateFb() {
            sendFrame();
        }

        void blinkOn(){
            sendCMD(HT1632_CMD_BLON);
        }

        void blinkOff(){
            sendCMD(HT1632_CMD_BLOFF);
        }
         
        void brightness(uint8_t val){
            
            sendCMD(HT1632_CMD_PWM | val);
        }

 
    
    private:

        inline void wrClock(){
            
            
            HT1632_PORT &= ~( 1 << HT1632_WR );
            
            //_delay_us(10);

            HT1632_PORT |= 1 << HT1632_WR;
            
            //_delay_us(10);
        }

        inline void enable(){
            HT1632_PORT &= ~(1 << HT1632_CS);
        }

        inline void disable(){
            HT1632_PORT |= 1 << HT1632_CS;
        }

        inline void setData(uint8_t data) {
            if(data != 0)
                 HT1632_PORT |= 1 << HT1632_DATA;
            else
                HT1632_PORT &= ~(1 << HT1632_DATA);
        }

        inline void sendBits(uint8_t bits, uint8_t msb){

            do{
                 setData(bits & msb);
                 wrClock();
            }while(msb >>=1);
       }

        inline void sendCMD( uint8_t cmd){
            enable();       

            sendBits(HT1632_ID_CMD, HT1632_ID_LEN);
            sendBits(cmd, HT1632_CMD_LEN);
            sendBits(0,1);
            
            disable();

        }

        inline void sendFrame(){
            enable();
            
            sendBits(HT1632_ID_WR, HT1632_ID_LEN);
            sendBits(0, HT1632_ADDR_LEN);
            for( int i = 0; i < 32; i++){
                sendBits(fb[i], HT1632_DATA_LEN);
            }

            disable();
        }

    private:
        uint8_t fb[32];
        
        uint8_t* PROGMEM font;
        uint8_t font_width;
        uint8_t font_height;


};



#endif
