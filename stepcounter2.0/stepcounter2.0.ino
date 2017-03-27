//import LED library
#include <Adafruit_NeoPixel.h> 
#ifdef __AVR__
  #include <avr/power.h>
#endif

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
  StopWatch();
  //StepCounter();
  //Encouragment();
  //NightDayMode();
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
      //If workout was long (30min?) PARTY LIGHTS
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
//  displaydigit1(d1,empty);
//  displaydigit2(d10, empty);
//  displaydigit3(d100, empty);
//  displaydigit4(d1000,empty);
  lastButtonState = buttonState;
}
  
//STEP COUNTER
void StepCounter(){
 switchState = digitalRead(tiltSensor); //switchState is high
 //Serial.println(switchState);
//  if (switchState != prevSwitchState) {
//    if (switchState == LOW) {
//      steps = (steps + 1);
//    }
//  }
  //Debouncing doesn't work
  if(switchState != prevSwitchState){
    tiltTime = millis();
    if(millis()-tiltTime > tiltDebounce){
    steps++;
   }
  }
  
  int temp = steps/2;
  int d1000=temp/1000;
  int d100=temp/100;
  int d10= temp/10;
  int d1= temp%10;
//  displaydigit1(d1,empty);
//  displaydigit2(d10, empty);
//  displaydigit3(d100, empty);
//  displaydigit4(d1000,empty);
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
void NightDayMode(){
  timer = 0;
  buttonState = digitalRead(A5);
  while (buttonState  = HIGH) {
    delay(100);
    timer++;
   }
  if (timer > 10) { //button has been pressed less than a second
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(204,0,0)); //red LEDs
        pixels.show();
      }
  }
}
void partyLights(){
   for(int j=0;j<8;j++){
  for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(204,153,255)); //light purple LEDs
      pixels.show();
      delay(3);
   }
   delay(30);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(255,128,0)); //orange LEDs
      pixels.show();
      delay(3);
   }
   delay(30);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,255,255)); //blue LEDs
      pixels.show();
      delay(3);
   }
      delay(30);
   for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(255,0,255)); //pink LEDs
      pixels.show();
      delay(3);
   }
   }
    delay(900);
    //turn off LEDS
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0)); //red LEDs
        pixels.show();
      }
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

