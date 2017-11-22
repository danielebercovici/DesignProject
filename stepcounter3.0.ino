//Import LED library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//LEDs
#//define PIN            A2
//changed to pin 10
const int ledPin = 10;
#define NUMPIXELS      60 //Num of LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, ledPin, NEO_GRB + NEO_KHZ800);

//Include humidity and temperature sensor library
#include <dht11.h>  
dht11 DHT;
#define DHT11_PIN 8

//Include OLED library
#define _Digole_Serial_I2C_ 
#include <DigoleSerial.h>

//I2C setup
#if defined(_Digole_Serial_I2C_)
#include <Wire.h> //import in order to use I2C
DigoleSerialDisp mydisp(&Wire,'\x27');  //I2C:Arduino UNO: SDA (data line) is on analog input pin 4, and SCL (clock line) is on analog input pin 5 on UNO and Duemilanove
#endif

int mode;

//if mode = 0; display is off
//else if mode = 1; display is on for a limited amount of time
//display temperature, humidity, steps
//else if mode = 2; timer mode is on

//want step function (accelerometer) to be running at all time
//want tilt sensor to be running all the time as well
//button is for stopwatch

//Accelerometer Pedometer initialization
const int xAxisPin = A3;
const int yAxisPin = A2;
const int zAxisPin = A1;
float xAvg, yAvg, zAvg;
float xVal[100] = {0};
float yVal[100] = {0};
float zVal[100] = {0};
int steps = 0;
int flag = 0;
float threshold = 80.0;

int timer = 0; //timer for LED button
const int tiltSensor = 11;
long tiltTime = 0;
long tiltDebounce = 50;
int switchState = HIGH;
int prevSwitchState = LOW;
char empty = '$';  

//Night time initialization 
int current;                             
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = HIGH;
unsigned long firstTime;
bool buttonPush=false;

//Stop watch initialization
const int buttonPin = 9;
unsigned long startTime, elapsedTime, elapsedHours, elapsedMinutes, elapsedSeconds;
int buttonPushCounter, buttonState = 0;  
int lastButtonState = 1;   

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(tiltSensor, INPUT);
  pinMode(ledPin, INPUT);
  pinMode(buttonPin, INPUT);
  pixels.begin(); 
  mydisp.begin();
  mydisp.setColor(255);
  calibrate();
}

//Function used to set up int (how often an event should be triggered)
//http://forum.arduino.cc/index.php?topic=5686.0
//boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
//{
// unsigned long currentMillis = millis();
// if(currentMillis - *lastMillis >= cycle)
// {
//   *lastMillis = currentMillis;
//   return true;
// }
// else
//   return false;
//}

void stopwatch(){
  unsigned long h, m, s, ms, over;

  buttonState = digitalRead(9);   //read the state the button, buttonState starts with LOW (pressed or not pressed)
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
      Serial.println(elapsedTime);
      elapsedSeconds = elapsedTime/1000L;
      elapsedMinutes = elapsedTime/60000L;
      elapsedHours = elapsedTime/3600000L;
      buttonPushCounter--;
      Serial.println("stop");
      //Serial.println((int) elapsedTime);
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(204,0,0)); //red LEDs
        pixels.show();
      }
      delay(900);
      //If workout was long (3 sec) rewarded w/ PARTY LIGHTS
      if(elapsedTime > 3000){
       partyLights();
      }
      //turn off LEDS
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //red LEDs
        pixels.show();
      }
      
      mydisp.clearScreen();
      mydisp.print("Time Elapsed: ");
      mydisp.println();

      if(elapsedHours<10){
        mydisp.print("0");
      }
      mydisp.print(elapsedHours);
      mydisp.print(":");
      if(elapsedMinutes < 10) {
        mydisp.print("0");
      }
      if(elapsedMinutes > 60) {
        m = elapsedMinutes%60L;
        mydisp.print(m);
      } else {
        mydisp.print(elapsedMinutes);
      }
      mydisp.print(":");
      
      if(elapsedSeconds < 10){
        mydisp.print("0");
      }
      if(elapsedSeconds > 60) {
        s = elapsedSeconds%60L;
        mydisp.print(s);
      } else {
        mydisp.print(elapsedSeconds);
      }
          }
            }
    // Delay to avoid bouncing
    //delay(10);
    delay(5000);
    //mydisp.moduleOff();
 
  lastButtonState = buttonState;
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


void calibrate() {
  digitalWrite(13, HIGH);
  float sum, sum1, sum2 = 0;

  for(int i=0; i<100; i++) {
    xVal[i] = float(analogRead(xAxisPin));
    sum = xVal[i] + sum;
  }
  xAvg = sum/100.0;
  delay(100);
  Serial.println(xAvg);

  for(int j=0; j<100; j++) {
    yVal[j] = float(analogRead(yAxisPin));
    sum1 = yVal[j] + sum1;
  }
  yAvg = sum1/100.0;
  delay(100);
  Serial.println(yAvg);

  for(int t=0; t<100; t++) {
    zVal[t] = float(analogRead(zAxisPin));
    sum2 = zVal[t] + sum2;
  }
  zAvg = sum2/100.0;
  delay(100);
  Serial.println(zAvg);

  digitalWrite(13,LOW);
}

void pedometer(){
  int acceleration = 0;
  float totalAccVector[100], totalAverage[100] = {0};
  float xAcceleration[100], yAcceleration[100], zAcceleration[100] = {0};

  for(int i=0; i<100; i++){
  //Read x axis and store value intro an array
  xAcceleration[i] = float(analogRead(xAxisPin));
  delay(1);

  //Read y axis
  yAcceleration[i] = float(analogRead(yAxisPin));
  delay(1);

  //Read z axis
  zAcceleration[i] = float(analogRead(zAxisPin));
  delay(1);

  //Magnitude of acceleration is sqrt of acx^2 + acy^2 + acz^2
  totalAccVector[i] = sqrt(((xAcceleration[i]-xAvg) * (xAcceleration[i]-xAvg)) + ((yAcceleration[i]-yAvg) * (yAcceleration[i]-yAvg)) + ((zAcceleration[i]-zAvg) * (zAcceleration[i]-zAvg))); 
  totalAverage[i] = (totalAccVector[i] + totalAccVector[i-1])/2;
    
  Serial.println(totalAverage[i]);
  delay(200);

  //Calculate the steps
  if(totalAverage[i] > threshold && flag == 0) {
    steps = steps +1;
    flag = 1;
  } else if (totalAverage[i] > threshold && flag == 1) {
    // do nothing
  } if(totalAverage[i] < threshold && flag == 1) {
    flag = 0;
  }

  //Print steps
  Serial.println('\n');
  Serial.print("steps=");
  Serial.println(steps);
  }
  delay(1000);
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

//Tilt sensor used to switch modes
//void tiltSensor(){
// mydisp.clearScreen();
// switchState = digitalRead(tiltSensor); //switchState is high
// //Serial.println(switchState);
//  if (switchState != prevSwitchState) {
//    if (switchState == LOW) {
//      steps = (steps + 1);
//    }
//  }
//  
//  int temp = steps;
//  //mydisp.clearScreen();
//  Serial.print(temp);
//  mydisp.clearScreen();
//  mydisp.print("Steps:");
//  mydisp.print(temp);
//  delay(500);
//  mydisp.clearScreen();
//  
//  //If you reached daily recommended number of steps (10) rewarded w/ PARTY LIGHTS
//  if(temp>10){
//    partyLights();
//    steps=0;
//    } 
//prevSwitchState = switchState;
//  }
  
void loop() {
stopwatch(); 
}


