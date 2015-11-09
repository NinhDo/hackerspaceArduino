#include <Wire.h>

// For joysticks
int x = 4;
int y = 4;


// LED Matrix
// ROWS
int datapin = 2; 
int clockpin = 3;
int latchpin = 4;

byte data = 0;
byte data2 = 0;
byte data3 = 0;
byte data4 = 0;

void setup() {
  Wire.begin(7);
  Wire.onReceive(receiveEvent);

  pinMode(latchpin, OUTPUT);
}

void loop() {
  marquee();
}

void shiftWrite(int desiredPin, boolean desiredState) {
  bitWrite(data,desiredPin,desiredState);
  shiftOut(datapin, clockpin, data);
}

void shiftWrite2(int desiredPin, boolean desiredState) {
  bitWrite(data2,desiredPin,desiredState);
  shiftOut(datapin, clockpin, data2);
}

void shiftWrite3(int desiredPin, boolean desiredState) {
  bitWrite(data3,desiredPin,desiredState);
  shiftOut(datapin, clockpin, data3);
}

void shiftWrite4(int desiredPin, boolean desiredState) {
  bitWrite(data4,desiredPin,desiredState);
  shiftOut(datapin, clockpin, data4);
}

void marquee() {
  int delayTime = 200;

  for(int index = 0; index < 8; index++) {
    digitalWrite(latchpin, 0);
    shiftWrite(index, 1);
    shiftWrite2(index, 1);
    shiftWrite3(index, 1);
    shiftWrite4(index, 1);
    digitalWrite(latchpin, 1);
    delay(delayTime);
    digitalWrite(latchpin, 0);
    shiftWrite(index, 0);
    shiftWrite2(index, 0);
    shiftWrite3(index, 0);
    shiftWrite4(index, 0);
    digitalWrite(latchpin, 1);
  }
}


void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

 //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}



void receiveEvent(int howMany) {
  while(1 < Wire.available()) {
    x = Wire.read();
  }
  y = Wire.read();
}

void sendToSlave() {
  
}
