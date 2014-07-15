#include <util/delay.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>


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
       
        void putPixel(int8_t x, int8_t y, char color){

            int addr;
            int offset;

            if(x < 0 || x > 31) return;
            if(y < 0 || y > 7) return;

             // 10 , 3
            addr = y + (x & 0xf8); 
           

            offset = 8 - (x + 1) & 0x07 ;

            if (color)
             fb[addr] |= 1 << offset;
            else
             fb[addr] &= ~(1 << offset);
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

        void wrClock(){
            
            
            HT1632_PORT &= ~( 1 << HT1632_WR );
            
            //_delay_us(10);

            HT1632_PORT |= 1 << HT1632_WR;
            
            //_delay_us(10);
        }

        void enable(){
            HT1632_PORT &= ~(1 << HT1632_CS);
        }

        void disable(){
            HT1632_PORT |= 1 << HT1632_CS;
        }

        void setData(uint8_t data) {
            if(data != 0)
                 HT1632_PORT |= 1 << HT1632_DATA;
            else
                HT1632_PORT &= ~(1 << HT1632_DATA);
        }

        void sendBits(uint8_t bits, uint8_t msb){

            do{
                 setData(bits & msb);
                 wrClock();
            }while(msb >>=1);
       }

        void sendCMD( uint8_t cmd){
            enable();       

            sendBits(HT1632_ID_CMD, HT1632_ID_LEN);
            sendBits(cmd, HT1632_CMD_LEN);
            sendBits(0,1);
            
            disable();

        }

        void sendFrame(){
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

};




void delay_ms(int d){
    int i;
    for (i = 0 ; i < d; ++i){
        _delay_ms(1);
    }
}

LedMatrix_t lm;

int main(){


    lm.clear();
     lm.brightness(5);

    while(1){
        lm.clear();
        //_delay_ms(20);
        for (int i = 0; i < 50; i++){
            int x = rand()%32;
            int y = rand()%8;
            lm.putPixel(x,y,1);
        }
        lm.updateFb();
        _delay_ms(20);

    }


    return 0;

}
