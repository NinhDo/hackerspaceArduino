#include <LiquidCrystal.h>
#include <Wire.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int i = 1;
unsigned int score = 0;
String snake1 = "_______.-===-.____.---.";//line0
String snake2 = "  '-.___.---.______.--,'"; //line1


void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  lcd.begin(16, 2);
  lcd.print("Andre");
  lcd.write(0xE3);
  lcd.print(" og Ninh:");
  lcd.setCursor(0, 1);
  lcd.print("SNAKE!");
  pinMode(6,OUTPUT);
  int test = 0;
  Serial.begin(9600);
}

void loop() {
  while (i == 1){
    delay(1000);
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
    /*  lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Score: ");
      lcd.print(score);
      delay(500);*/  
  }
}

void receiveEvent(int howMany) {
  int x = Wire.read();
  int appleTimer;
  int appleTimer2;
  int tempScore = 50;
  i = 1;
  lcd.clear();
  lcd.setCursor(0,0);
  Serial.println(x);
  switch (x) {
    case 0:
      lcd.print("Game start:");
      delay(2000);
      break;
    case 1:
      tempScore = 50;
      appleTimer= millis();
      appleTimer2= millis();
      while (appleTimer > 10000){
        if(millis()-appleTimer2 > 1000){
          tempScore = tempScore-5;
          appleTimer2 =millis();
        }
        if(x==1){
          break;
        }
      }
      score = score+tempScore;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Score: ");
      lcd.print(score);
      break;


    case 2:
      lcd.print("GAME OVER");
      break;

    case 3:
      lcd.print("Outlook good");
      break;

    case 4:
      lcd.print("Unsure");
      break;

    case 5:
      lcd.print("Ask again");
      break;

    case 6:
      lcd.print("Doubtful");
      break;

    case 7:
      lcd.print("No");
      break;
    default:
      lcd.print("?????");
      break;
  }
}

boolean readyToCompare(uint8_t lydsekvensAlfa[],uint8_t lydsekvensBeta[]){
    if ((lydsekvensAlfa[0] == lydsekvensBeta[0])&&(lydsekvensAlfa[1] == lydsekvensBeta[1])&&(lydsekvensAlfa[2] == lydsekvensBeta[2])){
        return true;
      }
    else {
      return false;
    }
}

