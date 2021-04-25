// 
//   FILE:  dht11_test1.pde
// PURPOSE: DHT11 library test sketch for Arduino
//


//#include <CapacitiveSensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_SENSOR_PIN 8
#define LEDPIN 7

//#define CS_CHARGEPIN 10
//#define CS_MEASUREPIN 9
#define BUTTON_PIN 11

#define ENDSTOP_OPENPIN 6
#define ENDSTOP_CLOSEPIN 5

#define MOTOR_OPENPIN  2
#define MOTOR_CLOSEPIN 4
#define MOTOR_PWMPIN 3
#define MOTOR_MAXSPEED 255



OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
//CapacitiveSensor cs = CapacitiveSensor(CS_CHARGEPIN,CS_MEASUREPIN); 
char manualMode = 0;
char measure = 0;
char openclose = 1;
float temperature = 0.0;



// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
	double RATIO = 373.15 / (273.15 + celsius);  // RATIO was originally named A0, possibly confusing in Arduino context
	double SUM = -7.90298 * (RATIO - 1);
	SUM += 5.02808 * log10(RATIO);
	SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
	SUM += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
	SUM += log10(1013.246);
	double VP = pow(10, SUM - 3) * humidity;
	double T = log(VP/0.61078);   // temp var
	return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
	double a = 17.271;
	double b = 237.7;
	double temp = (a * celsius) / (b + celsius) + log(humidity/100);
	double Td = (b * temp) / (a - temp);
	return Td;
}

boolean checkButton(){
  if(digitalRead(BUTTON_PIN) == HIGH )
     return false;
  return true;
  /*long csbut = cs.capacitiveSensor(30);
  if (csbut > 200)
     return true;
  return false;*/
}


void openDoor(){
  
  openclose = 1;
  //acccel
  digitalWrite(MOTOR_OPENPIN,1);
  digitalWrite(MOTOR_CLOSEPIN,0);
  int i = 0;
  /*while(digitalRead(ENDSTOP_OPENPIN) == HIGH || i < MOTOR_MAXSPEED){
    analogWrite(MOTOR_PWMPIN,i);
    ++i;
  }*/
   while(digitalRead(ENDSTOP_OPENPIN) == HIGH ){
     digitalWrite(MOTOR_PWMPIN,1);
  } 
  //analogWrite(MOTOR_PWMPIN,0);
  digitalWrite(MOTOR_PWMPIN,0);
    
}

void closeDoor(){
  
  openclose = 0;
  //acccel
  digitalWrite(MOTOR_OPENPIN,0);
  digitalWrite(MOTOR_CLOSEPIN,1);
  int i = 0;
  /*while(digitalRead(ENDSTOP_CLOSEPIN) == HIGH || i < MOTOR_MAXSPEED){
    analogWrite(MOTOR_PWMPIN,i);
    ++i;
  }*/
   while(digitalRead(ENDSTOP_CLOSEPIN) == HIGH ){
     digitalWrite(MOTOR_PWMPIN,1);
  } 
  //analogWrite(MOTOR_PWMPIN,0);
    digitalWrite(MOTOR_PWMPIN,0);
}

void checkDoor(){
  if (openclose){
    openDoor();
  }else{
    closeDoor();
  }
}


void toggleDoor() {
  if(openclose){
    closeDoor();
  }else{
    openDoor();
  }
}

void setManualMode(char m){
  manualMode = m;
  digitalWrite(LEDPIN,m);
}


void setup()
{
//cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
Serial.begin(115200);
 sensors.begin();

pinMode(LED_BUILTIN,OUTPUT);
digitalWrite(LED_BUILTIN,0);

pinMode(ENDSTOP_OPENPIN, INPUT);
pinMode(ENDSTOP_CLOSEPIN, INPUT);
pinMode(BUTTON_PIN, INPUT);

digitalWrite(ENDSTOP_OPENPIN,1);
digitalWrite(ENDSTOP_CLOSEPIN,1);
digitalWrite(BUTTON_PIN,1);

pinMode(MOTOR_OPENPIN,OUTPUT);
pinMode(MOTOR_CLOSEPIN,OUTPUT);
pinMode(MOTOR_PWMPIN,OUTPUT);
digitalWrite(MOTOR_OPENPIN,0);
digitalWrite(MOTOR_CLOSEPIN,0);
digitalWrite(MOTOR_PWMPIN,0);



pinMode(LEDPIN,OUTPUT);
digitalWrite(LEDPIN,0);
openDoor();
}

void loop()
{
  
  //Serial.println("\n");


  if (measure % 100 == 0){
      sensors.requestTemperatures();
      temperature = sensors.getTempCByIndex(0);
      checkDoor();
      Serial.println(temperature);
  }
  ++measure;
  
  if (checkButton()){
    if(manualMode){
      int i = 1;
      while(checkButton() && i < 200){
        delay(10);
        ++i;
      }
      if (i == 200){
        setManualMode(0);
      }else{
        //toggle Door
        toggleDoor();
      }
     delay(1000);
    }else{
     setManualMode(1);
      //toggle door
      toggleDoor();
     delay(1000);
    }
    
  } 
  
  if(!manualMode){
    if(temperature >= 31){
      if(openclose == 0)
          openDoor();
    }
    if(temperature <= 25){
      if (openclose == 1)
          closeDoor();
    }
  }
  
  
  /*if (digitalRead(ENDSTOP_CLOSEPIN) == LOW || digitalRead(ENDSTOP_OPENPIN) == LOW){
     digitalWrite(LED_BUILTIN,1);
  }else{
     digitalWrite(LED_BUILTIN,0);
  }  */

/*
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
  }
*/
  


  //Serial.print("Humidity (%): ");
  //Serial.println((float)DHT11.humidity, 2);

  //Serial.print("Temperature (oC): ");
  //Serial.println((float)DHT11.temperature, 2);
  
 /*
  Serial.print("Dew Point (oC): ");
  Serial.println(dewPoint(DHT11.temperature, DHT11.humidity));

  Serial.print("Dew PointFast (oC): ");
  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));
*/
 //Serial.println(csbut);
  delay(100);
}
//
// END OF FILE
//
