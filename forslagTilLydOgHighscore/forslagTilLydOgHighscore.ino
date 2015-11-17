const int sound = 9;
int highscore = gethighscore();
int score;
void setup() {
  // put your setup code here, to run once
  pinMode(sound,OUTPUT);
  int notes[] = {262, 330, 350};
  
}

void loop() {
  // highscore:
   if (score > highscore) {
    lcd.write("NEW HIGHSCORE!");
    highscore = score;
    EEPROM.write(0, highscore); //write highscore to eeprom
  } 
  displayhighscore();
  
    
  }
}

void soundn(int whatSound){
  if (sound == true && mute == false){
    switch (whatSound){
    case 0: //up down right left
      tone(sound,0,100);
      break;
    case 1://eat aple
      tone(sound, 1, 570);
      break;
    case 2://RESET
      tone(sound, 2, 1000);
      break;
    case 3://dead
       tone(sound, 2, 100);
       delay(200);
       tone(sound, 1, 100);
       delay(200);
       tone(sound, 0, 100);
      break;
    case 4;//3...2...1...satrt
        tone(sound, 0, 700);
       delay(1000);
       tone(sound, 0, 700);
       delay(1000);
       tone(sound, 0, 700);
      break;
  }
}
int gethighscore() {
    highscore = EEPROM.read(0);
    return(highscore);
}
