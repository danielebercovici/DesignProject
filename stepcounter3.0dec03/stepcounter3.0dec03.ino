//Import LED library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//LEDs
#define ledPin 10
#define NUMPIXELS      15 //Num of LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, ledPin, NEO_GRB + NEO_KHZ800);

//Humidity and Temperature library
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

//Button Pins
#define buttonPin 9
#define buttonModePin 11

//Accelerometer Pins
#define xAxisPin A3
#define yAxisPin A2
#define zAxisPin A1

//Accelerometer Pedometer initialization
float xAvg, yAvg, zAvg;
float xVal[50], yVal[50], zVal[50] = {0};
int steps = 0;
byte flag = 0;
const int threshold = 85.0;

//Stop Watch initialization
unsigned long startTime, elapsedTime,elapsedHours, elapsedMinutes, elapsedSeconds;
byte buttonPushCounter, buttonState = 0;  
byte lastButtonState = 1;   

//Switch Event Button
// Button timing variables
const int debounce = 20;          // debounce period
const int DCgap = 280;            // max ms between clicks for a double click event
const int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
const int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button Variables
boolean buttonVal = HIGH;   // value read from button
boolean buttonLast = HIGH;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already


void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(ledPin, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(buttonModePin, INPUT);
  digitalWrite(buttonModePin, HIGH );
  pixels.begin(); 
  mydisp.begin();
  mydisp.setColor(255);
  mydisp.backLightOff();
  calibrate();
}

//function to switch events/modes depending on the push to the push-button
//http://forum.arduino.cc/index.php?topic=14479.0
int checkButton() {    
   int event = 0;
   buttonVal = digitalRead(buttonModePin);
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce) { //if button is pressed down
       downTime = millis(); 
       ignoreUp = false; 
       waitForUp = false; 
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce) {        
       if (not ignoreUp) {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2) {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast){
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime) {
           if (not longHoldEventPast) {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
}

void stopwatch(){
  mydisp.clearScreen();
  unsigned long h, m, s;
  
  buttonState = digitalRead(9);
  Serial.print(buttonState);
  if (buttonState == lastButtonState) {
    if (buttonState == HIGH && buttonPushCounter==0) {
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,150,0)); //green LEDs
        pixels.show();
      }
      delay(900);
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //turn off LEDs
        pixels.show();
      }
      startTime = millis();
      buttonPushCounter++;
      mydisp.backLightOn();
      mydisp.setFont(51);
      mydisp.println();
      mydisp.print("  00:00:00");
      Serial.println("go");
      delay(2000);
      mydisp.backLightOff();
    } else if (buttonState == HIGH && buttonPushCounter==1) {
      elapsedTime =   millis() - startTime;
      elapsedSeconds = (elapsedTime/1000L);
      elapsedMinutes = elapsedTime/60000L;
      elapsedHours = elapsedTime/3600000L;
      buttonPushCounter--;
      Serial.println("stop");
      Serial.println(elapsedTime); 
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(204,0,0)); //red LEDs
        pixels.show();
      }
      delay(900);
      //If workout was long (30 sec) rewarded w/ PARTY LIGHTS
      if(elapsedTime > 30000){
       partyLights();
      }
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //turn off LEDS
        pixels.show();
      }

      mydisp.clearScreen();
      mydisp.backLightOn();
      mydisp.setFont(51);
      mydisp.println();

      if(elapsedHours<10){
        mydisp.print("  0");
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
        if(s < 10) {
          mydisp.print("0");
        }
        mydisp.print(s);
      } else {
        mydisp.print(elapsedSeconds);
      }
      delay(3000);
      mydisp.backLightOff();
     }
    }
    // Delay to avoid bouncing
    delay(10);
  lastButtonState = buttonState;
}


//HUMIDITY AND TEMPERATURE 
void humidityTemp(){
   mydisp.clearScreen();
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
      mydisp.backLightOn();
      mydisp.setFont(18);
      mydisp.println(" ");
      mydisp.print("H ");
      mydisp.print(DHT.humidity);
      mydisp.println("%");
      mydisp.print("T ");
      mydisp.print(DHT.temperature);
      mydisp.print(char(176));
      mydisp.println("C");
      delay(3500);
      mydisp.backLightOff();
}

void calibrate() {
  digitalWrite(13, HIGH);
  float sum, sum1, sum2 = 0;

  for(int i=0; i<50; i++) {
    xVal[i] = float(analogRead(xAxisPin));
    sum = xVal[i] + sum;
  }
  xAvg = sum/50.0;
  delay(100);
  Serial.println(xAvg);

  for(int j=0; j<50; j++) {
    yVal[j] = float(analogRead(yAxisPin));
    sum1 = yVal[j] + sum1;
  }
  yAvg = sum1/50.0;
  delay(100);
  Serial.println(yAvg);

  for(int t=0; t<50; t++) {
    zVal[t] = float(analogRead(zAxisPin));
    sum2 = zVal[t] + sum2;
  }
  zAvg = sum2/50.0;
  delay(100);
  Serial.println(zAvg);

  digitalWrite(13,LOW);
}

int pedometer(){
  int acceleration = 0;
  float totalAccVector[50], totalAverage[50] = {0};
  float xAcceleration[50], yAcceleration[50], zAcceleration[50] = {0};

  for(int i=0; i<50; i++) {
  //Read x axis 
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

  //Serial.println(totalAverage[i]);
  delay(10);

  //Calculate the steps
  if(totalAverage[i] > threshold && flag == 0) {
    steps = steps +1;
    flag = 1;
  } else if (totalAverage[i] > threshold && flag == 1) {
    // do nothing
  } if(totalAverage[i] < threshold && flag == 1) {
    flag = 0;
  }

  //Reset steps after reaching 5000
  if(steps > 5000){
    steps = 0;
  }
  
  return(steps);
  }
  delay(100);
}

void returningSteps(){
//  Serial.println("in returning steps");
//  Serial.println(steps);
  mydisp.clearScreen();
  mydisp.backLightOn();
  mydisp.setFont(51);
  mydisp.println(" ");
  mydisp.println("Steps");
  mydisp.println(steps);
  delay(3000);
  mydisp.backLightOff();
}

void NightMode(){
    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(255,255,255)); //white LEDs
        pixels.show();
     }
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
 
void loop() {
  pedometer();
  int b = checkButton();
    if (b == 1) {
      stopwatch();
    }
    if (b == 2) {
      humidityTemp();
    }
    if (b == 3) {
      returningSteps();
    }
    if (b == 4) {
      NightMode();
    }
}


