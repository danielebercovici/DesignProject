#include <Adafruit_NeoPixel.h> //import LED library
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <dht11.h> //for humidity and temperature 
dht11 DHT;

#define _Digole_Serial_I2C_ //for OLED
#include <DigoleSerial.h>
#define DHT11_PIN 8 //humidity and temperature intialization 

//I2C setup
#if defined(_Digole_Serial_I2C_)
#include <Wire.h> //import in order to use I2C
DigoleSerialDisp mydisp(&Wire,'\x27');  //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#endif

//LEDs
#define PIN            A2 //LED pin
#define NUMPIXELS      60 //Num of LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int timer = 0; //timer for LED button
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

//Night time initialization 
int current;                             
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = HIGH;
unsigned long firstTime;
bool buttonPush=false;


const unsigned char PROGMEM umbrella []{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xbf, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 
0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 
0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xfc, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x7f, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 
0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xf7, 0xff, 0xfe, 0xdf, 
0xff, 0xff, 0xff, 0xff, 0xfd, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xfe, 0xff, 
0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xfb, 0xfe, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xbe, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xfa, 0xff, 
0xff, 0xff, 0xff, 0xfb, 0xef, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xf7, 
0xff, 0xff, 0xff, 0xfd, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xdf, 0xff, 
0xff, 0xff, 0xff, 0xfb, 0xff, 0xdf, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xdf, 0xef, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xbe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbe, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7d, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

void setup() {
  Serial.begin(9600);
  
  //initialize tiltsensor
  pinMode(tiltSensor, INPUT);
  // This initializes LED
  pixels.begin(); 
  //initialize Display
  mydisp.begin();
  mydisp.setColor(255);
  //mydisp.drawBitmap(,*umbrella);
}

void loop() {
  StopWatch();
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
    delay(10);
 
  lastButtonState = buttonState;
}
  
//STEP COUNTER
void StepCounter(){
 mydisp.clearScreen();
 switchState = digitalRead(tiltSensor); //switchState is high
 //Serial.println(switchState);
  if (switchState != prevSwitchState) {
    if (switchState == LOW) {
      steps = (steps + 1);
    }
  }
  
  int temp = steps;
  //mydisp.clearScreen();
  Serial.print(temp);
  mydisp.clearScreen();
  mydisp.print("Steps:");
  mydisp.print(temp);
  delay(500);
  mydisp.clearScreen();
  
  //If you reached daily recommended number of steps (10) rewarded w/ PARTY LIGHTS
  if(temp>10){
    partyLights();
    steps=0;
    } 
prevSwitchState = switchState;
  }


//ENCOURAGMENT
void Encouragment(){
  writeGoodJob();
  writeWeather();
} 

void writeGoodJob(){
  mydisp.clearScreen();
  mydisp.print("GOOD JOB!");
  delay(1000);
  mydisp.clearScreen();
  }

void writeWeather(){
  mydisp.clearScreen();
  mydisp.println("Cloudy with a");
  mydisp.println("chance of rain!");
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
  chk = DHT.read(DHT11_PIN);
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
   if(DHT.humidity > 40){
     mydisp.print("HUMIDITY: ");
     mydisp.print(DHT.humidity);
     mydisp.println("%");
     mydisp.print("TEMP: ");
     mydisp.print(DHT.temperature);
     mydisp.println("C");
     mydisp.print("Bring an umbrella!");
   } else if(DHT.temperature > 20){
     mydisp.print("HUMIDITY: ");
     mydisp.print(DHT.humidity);
     mydisp.println("%");
     mydisp.print("TEMP: ");
     mydisp.print(DHT.temperature);
     mydisp.println("C");
     mydisp.print("It's a nice day to go out for a ");
     mydisp.println("jog!");
   }else if(DHT.temperature < 0){
     mydisp.print("HUMIDITY: ");
     mydisp.print(DHT.humidity);
     mydisp.println("%");
     mydisp.print("TEMP: ");
     mydisp.print(DHT.temperature);
     mydisp.println("C");
     mydisp.print("Dress up warmly! ");
   } else {
     mydisp.print("HUMIDITY: ");
     mydisp.print(DHT.humidity);
     mydisp.println("%");
     mydisp.print("TEMP: ");
     mydisp.print(DHT.temperature);
     mydisp.println("C");
   }
  delay(1000);
}


