#include <Wire.h>

// DEFINE STUFF
const boolean COFF = 0;
const boolean CON = 1;
const boolean ROFF = 1;
const boolean RON = 0;
const boolean LISTEN = 0;
const boolean DONTLISTEN = 1;

// FPS
unsigned long currentFrame = 0;

// Buzzer
uint8_t buzzer = 13;

// RESTARTBUTTON
uint8_t restartButton = 5;
boolean restartButtonPressed = 0;

// For joysticks
int lowDead = 412;
int highDead = 612;

// Joystick
uint8_t xPort = A0;
uint8_t yPort = A1;
uint8_t button1 = 9;
boolean buttonPress1 = 0;

uint8_t x = 7;
uint8_t y = 7;
int aX;
int aY;

// 0, 1, 2, 3; UP, DOWN, LEFT, RIGHT
uint8_t direction = 0;
const uint8_t UP = 0;
const uint8_t DOWN = 1;
const uint8_t LEFT = 2;
const uint8_t RIGHT = 3;

// LED Matrix
uint8_t datapin = 6;
uint8_t clockpin = 7;
uint8_t latchpin = 8;

byte data = 0;
byte data2 = 0;
byte data3 = 0;
byte data4 = 0;

// GAME STUFF
boolean running  = true;
unsigned long timeSinceRestart = 1500;

// SNAKE
int snake[256][2];
int length = 3;
uint8_t speed = 100;

int apple[] = { -1, -1};

// SETUP
void setup() {
  Wire.begin();
  pinMode(latchpin, OUTPUT);
  pinMode(xPort, INPUT);
  pinMode(yPort, INPUT);
  pinMode(button1, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(restartButton, INPUT);

  restartGame();
}

// LOOP
void loop() {
  // NEW VALUES FROM JOYSTICK
  aX = analogRead(xPort);
  aY = analogRead(yPort);
  //buttonPress1 = digitalRead(button1);
  restartButtonPressed = digitalRead(restartButton);

  // CHANGE DIRECTION
  if (aX < lowDead && direction != LEFT) {
    direction = RIGHT;
  } else if (aX > highDead && direction != RIGHT) {
    direction =  LEFT;
  } else if (aY < lowDead && direction != DOWN) {
    direction = UP;
  } else if (aY > highDead && direction != UP) {
    direction = DOWN;
  }
  if (running) {
    if (currentFrame % speed == 0) updatePosition();
  }

  if(restartButtonPressed == 1 && (timeSinceRestart > 1000 || !running)) {
    restartGame();
  }


  refreshMatrix();
}

void restartGame() {
  timeSinceRestart = 0;
  for (int i = 0; i < 256; i++) {
    snake[i][0] = -1;
    snake[i][1] = -1;
  }
  for (int i = 0; i < length; i++) {
    snake[i][0] = 9;
    snake[i][1] = 8;
  }
  x = 8;
  y = 7;
  refreshMatrix();
  running = true;
}

void updatePosition() {
  if (direction == UP) {
    y--;
  } else if (direction == DOWN) {
    y++;
  } else if (direction == LEFT) {
    x--;
  } else if (direction == RIGHT) {
    x++;
  }
  if (x < 0 || x > 15 || y < 0 || y > 15) {
    running = false;
    if (direction == UP) {
      y++;
    } else if (direction == DOWN) {
      y--;
    } else if (direction == LEFT) {
      x++;
    } else if (direction == RIGHT) {
      x--;
    }
  }
}

void refreshMatrix() {
  // Sets LED ON
  digitalWrite(latchpin, LISTEN);
  (x < 8) ? colLeft(x, CON) : colRight(x % 8, CON);
  (y < 8) ? rowTop(y, RON) : rowBot(y % 8 , RON);
  shiftAll();
  digitalWrite(latchpin, DONTLISTEN);
  // Sets all LEDs OFF
  digitalWrite(latchpin, LISTEN);
  for (int i = 0; i < 8; i++) {
    rowTop(i, ROFF);
    rowBot(i, ROFF);
    colLeft(i, COFF);
    colRight(i, COFF);
  }
  shiftAll();
  digitalWrite(latchpin, DONTLISTEN);
  currentFrame++;
  timeSinceRestart++;
}

// STUFF FOR SHIFT REGISTERS
void rowTop(int desiredPin, boolean desiredState) {
  bitWrite(data, desiredPin, desiredState);
}

void rowBot(int desiredPin, boolean desiredState) {
  bitWrite(data2, desiredPin, desiredState);
}

void colLeft(int desiredPin, boolean desiredState) {
  bitWrite(data3, desiredPin, desiredState);
}

void colRight(int desiredPin, boolean desiredState) {
  bitWrite(data4, desiredPin, desiredState);
}

void shiftAll() {
  shiftOut(datapin, clockpin, data4);
  shiftOut(datapin, clockpin, data3);
  shiftOut(datapin, clockpin, data2);
  shiftOut(datapin, clockpin, data);
}


void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}

void sendToSlave(int x) {
  Wire.beginTransmission(8);
  Wire.write(x);
  Wire.endTransmission();
}
