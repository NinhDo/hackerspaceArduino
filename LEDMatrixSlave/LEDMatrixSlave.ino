#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sensorValueX = 0;
int sensorValueY = 0;

int aX = A1;
int aY = A0;

int x = 4;
int y = 4;

int lowDead = 412;
int highDead = 612;

String lowerLine = "FREE BUTTS! ";
int scrollIndex = 0;
boolean scrolling = true;

void setup() {
  Wire.begin(10);
  Wire.onReceive(receiveEvent); 
  lcd.begin(16, 2);
}

void loop() {
  calcXY();
  writeLCD(); 
  delay(25);
}

void receiveEvent(int howMany) {
}

void calcXY() {
  sensorValueX = analogRead(aX);
  sensorValueY = analogRead(aY);
  
  //x = map(sensorValueX, 100, 924, 7, 0);
  //y = map(sensorValueY, 50, 974, 7, 0);
  
  if(sensorValueX < lowDead && x != 7) {
    x++;
  } else if (sensorValueX > highDead && x != 0) {
    x--;
  }
  if(sensorValueY < lowDead && y != 0) {
    y--;
  } else if (sensorValueY > highDead && y != 7) {
    y++;
  }
  sendToMaster();
}

void writeLCD() {
  if (lowerLine.length() < 16) {
    for(int i = 0; i < 16 - lowerLine.length(); i++) {
      lowerLine += " ";
    }
  }
  lcd.setCursor(0, 0);
  
  int milli = int(millis() / 10) % 100;
  int seconds = int(millis() / 1000) % 60;
  int minutes = int(millis()/ 60000) % 60;
  
  if(minutes < 10) lcd.print(0);
  lcd.print(minutes);
  lcd.print(":");
  if(seconds < 10) lcd.print(0);
  lcd.print(seconds);
  lcd.print(".");
  if(milli < 10) lcd.print(0);
  lcd.print(milli);

  lcd.setCursor(0, 1);
  //int charIndex = scrollIndex % lowerLine.length;
  int tempIndex = scrollIndex / 25 % lowerLine.length();
  for (int i = 0; i < lowerLine.length(); i++) {
    char c = lowerLine.charAt((i + tempIndex) % lowerLine.length());
    lcd.print(c);
  }
  scrollIndex++;
}

void sendToMaster() { 
  Wire.beginTransmission(7);
  Wire.write(x);
  Wire.write(y);
  Wire.endTransmission();
}
