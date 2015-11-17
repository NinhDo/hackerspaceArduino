#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String snake1 = "_______.-===-.____.---.";//line0
String snake2 = "  '-.___.---.______.--,'"; //line1

unsigned int score = 0;
boolean hasBegun = false;
boolean running = false;
boolean gameStartNotRun = true;
boolean lost = false;
uint8_t tempScore = 50;
unsigned long timeSinceLastApple = 0;
unsigned long timeSincePointReduction = 0;


void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  pinMode(6,OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Andre");
  lcd.print(" og Ninh:");
  lcd.setCursor(0, 1);
  lcd.print("SNAKE!");
}

void loop() {
  if(!hasBegun){
    delay(2500);
    lcd.clear(); 
    lcd.setCursor(16,0);
    lcd.print(snake1);
    lcd.setCursor(16,1);
    lcd.print(snake2);

    for (int scrollCounter = 0; scrollCounter < 38; scrollCounter++) { 
      lcd.scrollDisplayRight(); 
      delay(250);
    }
    lcd.clear();
  } 
  if(!running && gameStartNotRun) {
    gameStartNotRun = false;
    startGame();
  }
  if(running) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.print(score);
    if(millis() - timeSinceLastApple > 1500) {
      if(millis() - timeSincePointReduction > 1000) {
        timeSincePointReduction = millis();
        if(tempScore > 0) {
          tempScore -= 5;
        }
        if(tempScore < 0) {
          tempScore = 0;
        }
      }
    }
  }
  if(lost) {
    lcd.scrollDisplayLeft();
  }
  delay(250);
}

void receiveEvent(int howMany) {
  int x = Wire.read();
  Serial.println("GOT SUM: " + x);
  lcd.clear();
  lcd.setCursor(0,0);
  switch (x) {
    case 0:
      hasBegun = true;
      gameStartNotRun = true;
      break;
    case 1:
      score += tempScore;
      timeSinceLastApple = millis();
      break;

    case 2:
      tempScore = 50;
      break;

    case 3:
      running = false;
      lost = true;
      lcd.print("GAME OVER");
      lcd.setCursor(0, 1);
      lcd.print("Final Score: ");
      lcd.print(score);
      break;

    default:
      lcd.print("?????");
      break;
  }
}

void startGame() {
  score = 0;
  tempScore = 50;
  lost = false;
  hasBegun = true;
  unsigned long timeSinceLastApple = millis();
  unsigned long timeSincePointReduction = millis();
  delay(500);
  lcd.print("Game Start!");
  delay(2000);
  lcd.clear();
  lcd.print("3");
  delay(1000);
  lcd.clear();
  lcd.print("2");
  delay(1000);
  lcd.clear();
  lcd.print("1");
  delay(1000);
  sendToMaster(0);
  running = true; 
}

void sendToMaster(byte x) {
  Wire.beginTransmission(7);
  Wire.write(x);
  Wire.endTransmission();
}