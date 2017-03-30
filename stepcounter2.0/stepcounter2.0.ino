//import LED library
#include <Adafruit_NeoPixel.h> 
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <dht.h>
dht DHT;
#define _Digole_Serial_I2C_ 
#include <DigoleSerial.h>
//I2C setup
#if defined(_Digole_Serial_I2C_)
#include <Wire.h>
DigoleSerialDisp mydisp(&Wire,'\x27');  //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#endif

//HI!!!!!

const int tiltSensor = 13;
long tiltTime = 0;
long tiltDebounce = 50;
int steps = 0;
int switchState = HIGH;
int prevSwitchState = LOW;
char empty = '$'; 
int buttonPushCounter = 0;  
int buttonState = 0;         
int lastButtonState = 1;    
long startTime ;
long elapsedTime;
#define D1 11 //Digit 1
#define D10 12 //Digit 2
#define D100 10 //Digit 3
#define D1000 8 //Digit 4

#define PIN            A2 //LED pin
#define NUMPIXELS      60 //Num of LEDs
int timer =0; //timer for LED button

//Night time initialization 
int current;                             
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = HIGH;
unsigned long firstTime;
bool buttonPush=false;
//humidity and temperature intialization 
#define DHT11_PIN 8
//intialize LED library
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  
  //Initialize segment display
  pinMode(2, OUTPUT);  //D
  pinMode(3, OUTPUT);  //G
  pinMode(4, OUTPUT);  //C
  pinMode(5, OUTPUT);  //F
  pinMode(6, OUTPUT);  //B
  pinMode(7, OUTPUT);  //A
  pinMode(9, OUTPUT);  //E

  //4 digits of the 7 segment display
  pinMode(D1, OUTPUT); 
  pinMode(D10, OUTPUT);
  pinMode(D100, OUTPUT);
  pinMode(D1000, OUTPUT);
  
  //initialize tiltsensor
  pinMode(tiltSensor, INPUT);
  // This initializes LED
  pixels.begin(); 
  //initialize Display
  mydisp.begin();
  mydisp.setColor(255);
}

void loop() {
  //StopWatch();
  //StepCounter();
  //Encouragment();
  //NightDayMode();
  //HumidityTemp();
}

//STOPWATCH
void StopWatch(){
  //read the state the button, buttonState starts with LOW
  buttonState = digitalRead(A3);

  if (buttonState != lastButtonState) {
    
    if (buttonState == HIGH && buttonPushCounter==0) {
      startTime = millis();
      buttonPushCounter++;
      Serial.println("go");
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,150,0)); //green LEDs
        pixels.show();
      }
      delay(900);
      //turn off LEDS
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //red LEDs
        pixels.show();
      }
    } else if (buttonState == HIGH && buttonPushCounter==1) {
      elapsedTime =   millis() - startTime;
      buttonPushCounter--;
      Serial.println("stop");
      Serial.println( (int)(elapsedTime)); 
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(204,0,0)); //red LEDs
        pixels.show();
      }
      delay(900);
      //If workout was long (3 sec) rewarded w/ PARTY LIGHTS
      if((int)elapsedTime>3000){
       partyLights();
      }
      //turn off LEDS
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //red LEDs
        pixels.show();
      }
      int temp = (int)(elapsedTime); 
      mydisp.clearScreen();
      mydisp.print("Time:");
      mydisp.print(temp);
    }
      }
    // Delay to avoid bouncing
    delay(50);
 
  lastButtonState = buttonState;
}
  
//STEP COUNTER
void StepCounter(){
  
 switchState = digitalRead(tiltSensor); //switchState is high
 //Serial.println(switchState);
  if (switchState != prevSwitchState) {
    if (switchState == LOW) {
      steps = (steps + 1);
    }
  }
  
  int temp = steps/2;
  mydisp.clearScreen();
  Serial.print(temp);
  mydisp.print("Steps:");
  mydisp.print(temp);
  //If you reached daily recommended number of steps (10) rewarded w/ PARTY LIGHTS
  if(temp>10){
    partyLights();
    steps=0;
    }
prevSwitchState = switchState;
  }

//void Filter(){
//  
//}

//ENCOURAGMENT
void Encouragment(){
  writeGoodJob();
} 
void writeGoodJob(){
  mydisp.clearScreen();
  mydisp.print("GOOD JOB!");
  delay(1000);
  mydisp.clearScreen();
  }
  
//NIGHT DAY MODE
void NightDayMode(){
  current = digitalRead(A3);

  // if the button state changes to pressed, remember the start time 
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }
  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;
  Serial.println(secs_held);
  // debouncing check
  if (millis_held > 50) {
    // check if the button was released since we last checked
    if (current == HIGH && previous == LOW) {

      // If the button was held for 3-6 seconds for the first time
      if (secs_held >= 1 && secs_held < 3 && buttonPush==false ) {
        //Turn on Night Mode
        for(int i=0;i<NUMPIXELS;i++){
              pixels.setPixelColor(i, pixels.Color(255,255,255)); //white LEDs
              pixels.show();
         }
         Serial.print("LIGHTS camera action");
        buttonPush=true;
      }
      // If the button was held for 3-6 seconds for the second time
      else if (secs_held >= 1 && secs_held < 3 && buttonPush==true ) {
          //turn off LEDS
          delay(700);
        for(int i=0;i<NUMPIXELS;i++){
              pixels.setPixelColor(i, pixels.Color(0,0,0)); 
              pixels.show();
         }
         Serial.print("Off");
        buttonPush=false;
      }
    }
  }

  previous = current;
  prev_secs_held = secs_held;
}
void partyLights(){
   for(int j=0;j<3;j++){
  for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(204,153,255)); //purple LEDs
      pixels.show();
      delay(2);
   }
   delay(30);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(255,128,0)); //orange LEDs
      pixels.show();
      delay(2);
   }
   delay(30);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,255,255)); //blue LEDs
      pixels.show();
      delay(2);
   }
      delay(20);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(255,0,255)); //pink LEDs
      pixels.show();
      delay(3);
   }
   }
    delay(800);
    //turn off LEDS
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0));
        pixels.show();
      }
  }
  
//HUMIDITY AND TEMPERATURE 
 void HumidityTemp(){
  // READ DATA
   int chk;
  //Error checking
  chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // display data (TODO: give some application)
   mydisp.clearScreen();
   mydisp.print("Humidity: ");
   mydisp.print(DHT.humidity);
   mydisp.print("%");
   mydisp.print("Temp: ");
   mydisp.print(DHT.temperature);
   mydisp.print("C");
      
//  Serial.print(DHT.humidity,1);
//  Serial.print(",\t");
//  Serial.println(DHT.temperature,1);
  delay(1000);
  
}
    


