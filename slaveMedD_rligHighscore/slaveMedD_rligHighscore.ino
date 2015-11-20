#include <LiquidCrystal.h>
#include <Wire.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String snake1 = "_______.-===-.____.---.";//line0
String snake2 = "  '-.___.---.______.--,'"; //line1

int highscore = 0;
int score = 0;
boolean hasBegun = false;
boolean running = false;
boolean gameStartNotRun = true;
boolean gameOver = false;
uint8_t tempScore = 50;
unsigned long timeSinceLastApple = 0;
unsigned long timeSincePointReduction = 0;
int i = 0;
int a = 0;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  pinMode(6,OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Andre");
  lcd.print(" og Ninh:");
  lcd.setCursor(0, 1);
  lcd.print("SNAKE!");
  highscore = getHighScore();
}

void loop() {
  
  if(gameOver) {
    resetAll();
    lcd.scrollDisplayLeft();
  }
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
    if(checkHighScore() == true){
      i = 10;
      a = 2;
    }
    if (i > 0){
      if (a >= 2){
        lcd.setCursor(0,1);
        lcd.print("NEW HIGHSCORE!!");
        a = 0;
      }
      else{
       lcd.clear();
       a++;
      }
      i--;
    }
    if(i <= 0){
      lcd.setCursor(0,1);
      lcd.print("Highscore: ");
      highscore = getHighScore();
      lcd.print(highscore);
    }
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
  
  delay(250);
}


void receiveEvent(int howMany) {
  int x = Wire.read();
  Serial.println("GOT SUM: " + x);
  lcd.clear();
  lcd.setCursor(0,0);
  
    if(x == 0){
      hasBegun = true;
      gameStartNotRun = true;
    }
    if(x== 1){
      score += tempScore;
      timeSinceLastApple = millis();
    }

    if(x== 2){
      tempScore = 50;
    }

    if(x == 3){
      running = false;
      gameOver = true;
      lcd.print("GAME OVER");
      lcd.setCursor(0, 1);
      lcd.print("Final Score: ");
      lcd.print(score);
      
      lcd.scrollDisplayLeft();
    }

    else{
      lcd.print("?????");
    }
  }

void startGame() {
  score = 0;
  tempScore = 50;
  gameOver = false;
  hasBegun = true;
  unsigned long timeSinceLastApple = millis();
  unsigned long timeSincePointReduction = millis();
  lcd.setCursor(1,0);
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
boolean checkHighScore() {
   if (score > highscore) {
   setHighScore();
    return true;
    }
   else{
      return false;
    }
}

int getHighScore() {
    highscore = EEPROM.read(0);
    return(highscore);
}

void setHighScore(){
  highscore = score;
  EEPROM.write(0, highscore); //write highscore to eeprom
}

void resetAll() {
 highscore = getHighScore();
 tempScore = 50;
 timeSinceLastApple = 0;
 timeSincePointReduction = 0;
 i = 0;

}

