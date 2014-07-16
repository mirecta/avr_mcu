#include <util/delay.h>



#include "matrix.h"





LedMatrix_t lm;

/*

#define FONT_4x6     1
#define FONT_5x7     2
#define FONT_5x8     3
#define FONT_8x8    16

*/

int main(){



    lm.clear();
    lm.brightness(3);
   // lm.setFont(FONT_5x7);
//    lm.putChar(6,0,'A');
//    lm.putPixel(0,0,1);
 //     lm.printText(0,0,"12:00");
 //     lm.updateFb();
    //lm.hScrollText(0,"Hello world !!!", 25, 50, 0);
    while(1){

    lm.setFont(FONT_4x6);
    lm.hScrollText(0,"Font 4x6", 25, 1, 0);
    lm.setFont(FONT_5x7);
    lm.hScrollText(0,"Font 5x7", 25, 1, 0);
    lm.setFont(FONT_5x8);
    lm.hScrollText(0,"Font 5x8", 25, 1, 0);
    lm.setFont(FONT_8x8);
    lm.hScrollText(0,"Font 8x8", 25, 1, 0);


     for(int i = 0; i <= 50 ; i++){
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
    delay_ms(200);
    //lm.clear();
    for (int i = 0; i < 32; i++){

      lm.line(i,0,31-i,7);
      lm.updateFb();
      _delay_ms(30);
   }
    for (int i = 0; i < 8; i++){

      lm.line(0,7-i,31,i);
      lm.updateFb();
      _delay_ms(40);
   }
    for (int i = 0; i < 32; i++){

      lm.line(i,0,31-i,7,0);
      lm.updateFb();
      _delay_ms(30);
   }
    for (int i = 0; i < 8; i++){

      lm.line(0,7-i,31,i,0);
      lm.updateFb();
      _delay_ms(40);
   }

   
  for(int i = 0; i < 20; i++){
     lm.clear();
     lm.circle(15,3,i);
     lm.updateFb();
      _delay_ms(30);
  }


}


    return 0;

}
