// include the library code:
#include <LiquidCrystal.h>
#include <util/delay.h>
#include <MenuSystem.h>

//pin A is PB0/53
#define ROT_PIN_A     53
//pin B is PG0/41
#define ROT_PIN_B     41

#define ROT_BUTT    28  

#define EXIT_BUTT   36

#define SELECT 0
#define EXIT   1


volatile int encoderMax = 100;
volatile int encoderMin = 0;
volatile int8_t rotaryEncoder = 0;
volatile uint8_t lastVal = 1;


ISR (PCINT0_vect){
//cli();

//i use low level accsess for increase speed
//is our pin ? PB0/53
  uint8_t val = (PINB & (1 << PB0))? 1 : 0;
  if((val == 0) && (lastVal == 1)){
    if((PING & (1 << PG0)) == 0){
      rotaryEncoder ++;
      if(rotaryEncoder > encoderMax)
        rotaryEncoder = encoderMin;
    }
    else
    { 
      rotaryEncoder --;
      if(rotaryEncoder < encoderMin)
        rotaryEncoder = encoderMax;
    }
  }
  lastVal = val;

//sei();
}

void rotarySetup(){
  pinMode(ROT_PIN_A, INPUT);
  pinMode(ROT_PIN_B, INPUT);
  pinMode(ROT_BUTT, INPUT);
  pinMode(EXIT_BUTT, INPUT);
  
  digitalWrite(ROT_PIN_A,HIGH);
  digitalWrite(ROT_PIN_B,HIGH);
  digitalWrite(ROT_BUTT,HIGH);
  digitalWrite(EXIT_BUTT,HIGH);
  //enable interrupt for pinchange
  PCICR |= 1 << PCIE0;
  PCMSK0 |= 1<<PCINT0;
  sei();
}

uint8_t readButtons(){
  
 uint8_t result = 0;
 if(digitalRead(ROT_BUTT))
     result |= 1 << SELECT;
     
 if( digitalRead(EXIT_BUTT))
     result |= 1 << EXIT;
     
 return result;
  
}




// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(26, 27, 25, 24, 23, 22);
MenuSystem ms;
Menu mm("");

Menu     mm_1("Nastavenie");
MenuItem mm_1_1("Datum a cas");
Menu     mm_1_2("Manual");
MenuItem mm_1_2_1("Cerpadlo");
MenuItem mm_1_2_2("Ventily");
MenuItem mm_1_2_3("Navrat");
MenuItem mm_1_3("Casovac");
MenuItem mm_1_4("Navrat");



// Menu callback function
// In this example all menu items use the same callback.

void on_datetime(MenuItem* p_menu_item)
{

}

void on_cerpadlo(MenuItem* p_menu_item)
{
  
}

void on_ventily(MenuItem* p_menu_item)
{
 
}

void on_navrat(MenuItem* p_menu_item){
ms.back();
}

void on_casovac(MenuItem* p_menu_item)
{
 
}



void menuSetup(){
 
   // Menu setup
  mm.add_menu(&mm_1);
  mm.add_item(&mm_1_1, &on_datetime);
  mm.add_menu(&mm_1_2);
  mm_1_2.add_item(&mm_1_2_1,&on_cerpadlo);
  mm_1_2.add_item(&mm_1_2_2,&on_ventily);
  mm_1_2.add_item(&mm_1_2_3,&on_navrat);
  
  mm.add_item(&mm_1_3, &on_casovac);
  mm.add_item(&mm_1_4, &on_navrat);
  

  ms.set_root_menu(&mm);
  
}


void p(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        lcd.print(buf);
}

void p(const __FlashStringHelper *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  lcd.print(buf);
}

int8_t preVal;


void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(20,4);
  lcd.print("test");
  lcd.clear();
  rotarySetup();
  menuSetup();
}



void loop(){


 
  
lcd.setCursor(0,3);
lcd.print(ms.get_current_menu()->get_selected()->get_name());

int8_t val = rotaryEncoder;
uint8_t butt = readButtons();

if (val > preVal)
     ms.next();
if (val < preVal)
     ms.prev();
     
 if(butt & (1<<SELECT))
     ms.select();
  if(butt & (1<<EXIT))
     ms.back();    
     
     

preVal = val;

delay(10);

}

