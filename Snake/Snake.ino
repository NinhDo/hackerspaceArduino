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

uint8_t x = 8;
uint8_t y = 7;
int aX;
int aY;

// 0, 1, 2, 3; UP, DOWN, LEFT, RIGHT
uint8_t direction = 1;
uint8_t movingDirection = 1;
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

boolean board[16][16];


// GAME STUFF
boolean running  = false;
unsigned long timeSinceRestart = 0;

// SNAKE
int snake[256][2];
int length = 3;
int defaultLength = 3;
int sx = -1;
int sy = -1;
uint8_t speed = 25;
  
int apple[] = { -1, -1};

// SETUP
void setup() {
  Wire.begin(7);
  Wire.onReceive(receiveEvent);
  pinMode(latchpin, OUTPUT);
  pinMode(xPort, INPUT);
  pinMode(yPort, INPUT);
  pinMode(button1, INPUT);
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
  if (aX < lowDead && direction != LEFT && movingDirection != LEFT) {
    direction = RIGHT;
  } else if (aX > highDead && direction != RIGHT && movingDirection != RIGHT) {
    direction =  LEFT;
  } else if (aY < lowDead && direction != DOWN && movingDirection != DOWN) {
    direction = UP;
  } else if (aY > highDead && direction != UP && movingDirection != UP) {
    direction = DOWN;
  }
  if (running) {
    if (currentFrame % speed == 0) updatePosition();
  }

  if(restartButtonPressed == 1 && (millis() - timeSinceRestart > 1000)) {
    restartGame();
  }

  refreshSnake();
  //refreshMatrix();
}

void restartGame() {
  // SEND GAME START
  timeSinceRestart = millis();
  length = defaultLength;
  direction =1;
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      board[row][col] = 0;
    }
  }
  for (int i = 0; i < 256; i++) {
    snake[i][0] = -1;
    snake[i][1] = -1;
  }
  for (int i = 0; i < length; i++) {
    snake[i][0] = 8;
    snake[i][1] = 7;
  }
  x = 8;
  y = 7;
  placeApple();
  //refreshMatrix();
  refreshSnake();
  sendToSlave(0);
  running = false;
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
    // SEND GAME OVER
    sendToSlave(3);
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
  movingDirection = direction;
  updateSnakePositions();
}

void updateSnakePositions() {
  if (apple[0] == x && apple[1] == y) {
    length++;
    sendToSlave(1);
    apple[0] = -1;
    apple[1] = -1;
    placeApple();
  }
  for (int segment = 1; segment < length; segment++) {
    if (snake[length - segment][0] != snake[length - 1 - segment][0]) 
      snake[length - segment][0] = snake[length - 1 - segment][0];
    if (snake[length - segment][1] != snake[length - 1 - segment][1]) 
      snake[length - segment][1] = snake[length - 1 - segment][1];
  }
  snake[0][0] = x;
  snake[0][1] = y;
  
  for(int segment = 1; segment < length; segment++) {
    if(snake[0][0] == snake[segment][0] && snake[0][1] == snake[segment][1]) {
      running = false;
      sendToSlave(3);
    }
  }
}

void placeApple() {
  boolean placedApple = false;
  int rX = -1;
  int rY = -1;
  while (true) {
    placedApple = true;
    rX = random(16);
    rY = random(16);
    for(int segment = 0; segment < length; segment++) {
      if(snake[segment][0] == rX && snake[segment][1] == rY) {
        placedApple = false;
        continue;
      }
    }
    if(placedApple)
      break;
  }
  apple[0] = rX;
  apple[1] = rY;
  if(running){
    sendToSlave(2);
  }
}

void wipeBoard() {
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      board[row][col] = 0;
    }
  }
}

void refreshSnake() {
  wipeBoard();
  for (int segment = 0; segment < length; segment++) {
    sx = snake[segment][0];
    sy = snake[segment][1];
    if(sx < 0 || sy < 0 || sx > 15 || sy > 15) {
      continue;
    }
    board[sy][sx] = 1;
  }
  board[apple[1]][apple[0]] = 1;

  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      if (board[row][col] == 0) continue;
      (col < 8) ? colLeft(col, board[row][col]) : colRight(col % 8, board[row][col]);
    } 
    (row < 8) ? rowTop(row, RON) : rowBot(row % 8 , RON);
    // Sets LED ON
    //digitalWrite(latchpin, LISTEN);
    PORTB &= ~_BV(PB4);
    shiftAll();
    //digitalWrite(latchpin, DONTLISTEN);
    PORTB |= _BV(PB4);
    // Sets all LEDs OFF
    //digitalWrite(latchpin, LISTEN);
    PORTB &= ~_BV(PB4);
    for (int i = 0; i < 8; i++) {
      rowTop(i, ROFF);
      rowBot(i, ROFF);
      colLeft(i, COFF);
      colRight(i, COFF);
    }
    shiftAll();
    //digitalWrite(latchpin, DONTLISTEN);
    PORTB |= _BV(PB4);
  }
  currentFrame++;
}


void refreshMatrix() {
  // Sets LED ON
  //digitalWrite(latchpin, LISTEN);
  PORTB &= ~_BV(PB4);
  (x < 8) ? colLeft(x, CON) : colRight(x % 8, CON);
  (y < 8) ? rowTop(y, RON) : rowBot(y % 8 , RON);
  shiftAll();
  //digitalWrite(latchpin, DONTLISTEN);
  PORTB |= _BV(PB4);
  // Sets all LEDs OFF
  //digitalWrite(latchpin, LISTEN);
  PORTB &= ~_BV(PB4);
  for (int i = 0; i < 8; i++) {
    rowTop(i, ROFF);
    rowBot(i, ROFF);
    colLeft(i, COFF);
    colRight(i, COFF);
  }
  shiftAll();
  //digitalWrite(latchpin, DONTLISTEN);
  PORTB |= _BV(PB4);
  currentFrame++;
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
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
  //int pinState;

  //digitalWrite(myDataPin, 0);
  //digitalWrite(myClockPin, 0);
  PORTD &= ~_BV(PD7);
  PORTE &= ~_BV(PE6);

  for (int i = 7; i >= 0; i--) {
    //digitalWrite(myClockPin, 0);
    PORTE &= ~_BV(PE6);

    if (myDataOut & (1 << i) ) {
      //pinState = 1;
      PORTD |= _BV(PD7);
    }
    else {
      //pinState = 0;
      PORTD &= ~_BV(PD7);
    }
    //digitalWrite(myDataPin, pinState);
    //digitalWrite(myClockPin, 1);
    //digitalWrite(myDataPin, 0);
    PORTE |= _BV(PE6);
    PORTD &= ~_BV(PD7);
  }

  //stop shifting
  //digitalWrite(myClockPin, 0);
  PORTE &= ~_BV(PE6);
}

void sendToSlave(byte x) {
  Wire.beginTransmission(8);
  Wire.write(x);
  Wire.endTransmission();
}

void receiveEvent(int howMany) {
  int x = Wire.read();
  if(x == 0) {
    running = true;
  }
}