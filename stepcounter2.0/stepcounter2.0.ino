//import LED library
#include <Adafruit_NeoPixel.h> 
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <dht.h>
dht DHT;


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

#define PIN            A4 //LED pin
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
#define DHT11_PIN A3
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
  buttonState = digitalRead(A5);

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
    }
      }
    // Delay to avoid bouncing
    delay(50);
 
      int temp = (int)(elapsedTime/1000); //changed to seconds instead (should change to seconds and minutes maybe xx:xx)
      int d1= temp%10;
      int d10= (temp/10)%10;
      int d100=(temp/100)%10;
      int d1000=(temp/1000)%10;
      
  //For threading, this should not be here
  //temp should be a global variable and display is another thread
  displaydigit1(d1,empty);
  displaydigit2(d10, empty);
  displaydigit3(d100, empty);
  displaydigit4(d1000,empty);
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
//  //Debouncing doesn't work
//  if(switchState != prevSwitchState){
//    tiltTime = millis();
//    if(millis()-tiltTime > tiltDebounce){
//    steps++;
//   }
//  }
  
  int temp = steps/2;
  //If you reached daily recommended number of steps (10) rewarded w/ PARTY LIGHTS
  if(temp>10){
    partyLights();
    steps=0;
    }
  int d1000=temp/1000;
  int d100=temp/100;
  int d10= temp/10;
  int d1= temp%10;
  displaydigit1(d1,empty);
  displaydigit2(d10, empty);
  displaydigit3(d100, empty);
  displaydigit4(d1000,empty);
prevSwitchState = switchState;
  }

//void Filter(){
//  
//}

//ENCOURAGMENT
void Encouragment(){
writeGood();
//delay(100);
//writeNice();
}

void writeNice(){
  displaydigit1(-1,'E');
  displaydigit2(-1, 'C');
  displaydigit3(-1,'I');
  displaydigit4(-1,'N');
  }
  
void writeGood(){
  displaydigit1(-1,'D');
  displaydigit2(-1, 'O');
  displaydigit3(-1,'O');
  displaydigit4(-1,'G');
  }
  
//NIGHT DAY MODE
void NightDayMode(){
  current = digitalRead(A5);

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
//        for(int i=0;i<NUMPIXELS;i++){
//              pixels.setPixelColor(i, pixels.Color(255,255,255)); //white LEDs
//              pixels.show();
//         }
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
  Serial.print(DHT.humidity,1);
  Serial.print(",\t");
  Serial.println(DHT.temperature,1);
  delay(1000);
}
    
void displaydigit1(int d1, char l){
    digitalWrite(D1, 0);
    digitalWrite(D10, 1);
    digitalWrite(D100, 1);
    digitalWrite(D1000, 1);
    display(d1,l);
}

void displaydigit2(int d10, char l){
    digitalWrite(D1, 1);
    digitalWrite(D10, 0);
    digitalWrite(D100, 1);
    digitalWrite(D1000, 1);
    display(d10,l);
 }
 
void displaydigit3(int d100, char l){
    digitalWrite(D1, 1);
    digitalWrite(D10, 1);
    digitalWrite(D100, 0);
    digitalWrite(D1000, 1);
    display(d100,l);
 }
 
 void displaydigit4(int d1000, char l){
    digitalWrite(D1, 1);
    digitalWrite(D10, 1);
    digitalWrite(D100, 1);
    digitalWrite(D1000, 0);
    display(d1000,l);
 }
 
 void display(int d, char l){
  if (l != empty){ 
      switch (l) {
      case('A'):
        digitalWrite(2, 0);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 1);
        digitalWrite(6, 1);
        digitalWrite(7, 1);
        digitalWrite(9, 1);
        break;
      case('C'):
        digitalWrite(2, 1);
        digitalWrite(3, 0);
        digitalWrite(4, 0);
        digitalWrite(5, 1);
        digitalWrite(6, 0);
        digitalWrite(7, 1);
        digitalWrite(9, 1);
        break;
      case('D'):
        digitalWrite(2, 1);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 0);
        digitalWrite(6, 1);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;
      case('E'):
        digitalWrite(2, 1);
        digitalWrite(3, 1);
        digitalWrite(4, 0);
        digitalWrite(5, 1);
        digitalWrite(6, 0);
        digitalWrite(7, 1);
        digitalWrite(9, 1);
        break;
      case('G'):
        digitalWrite(2, 1);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 1);
        digitalWrite(6, 0);
        digitalWrite(7, 1);
        digitalWrite(9, 1);
        break;
      case('H'):
        digitalWrite(2, 0);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 1);
        digitalWrite(6, 1);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;
      case('I'):
        digitalWrite(2, 0);
        digitalWrite(3, 0);
        digitalWrite(4, 1);
        digitalWrite(5, 0);
        digitalWrite(6, 1);
        digitalWrite(7, 0);
        digitalWrite(9, 0);
        break;
      case('N'):
        digitalWrite(2, 0);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 0);
        digitalWrite(6, 0);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;
      case('O'):
        digitalWrite(2, 1);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 0);
        digitalWrite(6, 0);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;

      case('T'):
        digitalWrite(2, 1);
        digitalWrite(3, 1);
        digitalWrite(4, 0);
        digitalWrite(5, 1);
        digitalWrite(6, 0);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;
      case('U'):
        digitalWrite(2, 1);
        digitalWrite(3, 0);
        digitalWrite(4, 1);
        digitalWrite(5, 0);
        digitalWrite(6, 0);
        digitalWrite(7, 0);
        digitalWrite(9, 1);
        break;
      default: //Y
        digitalWrite(2, 0);
        digitalWrite(3, 1);
        digitalWrite(4, 1);
        digitalWrite(5, 1);
        digitalWrite(6, 1);
        digitalWrite(7, 0);
        digitalWrite(9, 0);
        break;
    }
  }

    else  if (d != -1){
    switch (d) {
    case 1:
      digitalWrite(2, 0);
      digitalWrite(3, 0);
      digitalWrite(4, 1);
      digitalWrite(5, 0);
      digitalWrite(6, 1);
      digitalWrite(7, 0);
      digitalWrite(9, 0);
      break;
    case 2:
      digitalWrite(2, 1);
      digitalWrite(3, 1);
      digitalWrite(4, 0);
      digitalWrite(5, 0);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 1);
      break;
    case 3:
      digitalWrite(2, 1);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 0);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 0);
      break;
    case 4:
      digitalWrite(2, 0);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 1);
      digitalWrite(7, 0);
      digitalWrite(9, 0);
      break;
    case 5:
      digitalWrite(2, 1);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 0);
      digitalWrite(7, 1);
      digitalWrite(9, 0);
      break;
    case 6:
      digitalWrite(2, 1);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 0);
      digitalWrite(7, 1);
      digitalWrite(9, 1);
      break;
    case 7:
      digitalWrite(2, 0);
      digitalWrite(3, 0);
      digitalWrite(4, 1);
      digitalWrite(5, 0);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 0);
      break;
    case 8:
      digitalWrite(2, 1);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 1);
      break;
    case 9:
      digitalWrite(2, 0);
      digitalWrite(3, 1);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 0);
      break;
    default: //=0
      digitalWrite(2, 1);
      digitalWrite(3, 0);
      digitalWrite(4, 1);
      digitalWrite(5, 1);
      digitalWrite(6, 1);
      digitalWrite(7, 1);
      digitalWrite(9, 1);
    break;
  }
}
else{ //dont display anything (-1,empty)
    digitalWrite(D1, 1);
    digitalWrite(D10, 1);
    digitalWrite(D100, 1);
    digitalWrite(D1000, 1);
  }
  delay(1);
 }

