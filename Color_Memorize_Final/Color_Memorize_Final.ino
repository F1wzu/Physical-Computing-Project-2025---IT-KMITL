#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27,16,2);

int buttons[5] = {8, 9, 10, 11, 12};
int leds[5]    = {2, 3, 4, 5, 6};
boolean button[5] = {0, 0, 0, 0, 0};

int startBTN = 13;

int highScore = 0;

#define buzzer 7
#define levelsInGame 50

int bt_memorize[100];
int led_memorize[100];

boolean lost;
int game_play, level, stage;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 5; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }

  pinMode(startBTN, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
 
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("   Welcome To   ");
  lcd.setCursor(0,1);
  lcd.print("  Memory  Game  ");
  delay(2000);
  lcd.clear(); 

  randomSeed(analogRead(0));
}

void loop() {
  switch(stage){
  case 0:
    lcd.setCursor(0, 0); lcd.print("Press 6th Button");
    lcd.setCursor(0, 1); lcd.print(" To Start Game ");
    while(digitalRead(startBTN) == HIGH) {
      digitalRead(startBTN);
    }
    level=1; stage=1; game_play=1;
    break;
      
  case 1:
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Level: ");
    lcd.print(level);
 
    lcd.setCursor(0,1);
    lcd.print(" -- Memorize -- ");
    delay(1500);

    led_memorize[level] = random(0, 5);

    for (int i = 1; i <= level; i++) {
      int ledIndex = led_memorize[i];
      digitalWrite(leds[ledIndex], HIGH);
      playBuzzer(ledIndex+1);
      digitalWrite(leds[ledIndex], LOW);
      delay(400);
    }
    delay(500);
    stage = 2;
    break;
      
  case 2:
    stage = 3;
    lcd.setCursor(0, 1); 
    lcd.print("   -- Play --   "); 
    break;

  case 3:
    for (int i = 0; i < 5; i++) {
      button[i] = digitalRead(buttons[i]);
      if (button[i] == LOW) {
        bt_memorize[game_play] = i;
        digitalWrite(leds[i], HIGH);
        playBuzzer(i+1);
        while (button[i] == LOW) {
          button[i] = digitalRead(buttons[i]);
        }
        delay(50);
        digitalWrite(leds[i], LOW);
        game_play++;
        if (game_play-1 == level) {
          game_play = 1;
          stage = 4;
          break;
        }
      }
    }
    delay(10);
    break;

  case 4:
    lcd.setCursor(0, 1); 
    lcd.print("  Verification  ");
    delay(1000);

    lost = 0;
    for (int i = 1; i <= level; i++) {
      if (led_memorize[i] != bt_memorize[i]) {
        lost = 1;
        break;
      }
    }

    if (lost) stage = 5;
    else stage = 6;
    break;

  case 5:
    lcd.setCursor(0, 1); 
    lcd.print(" !! You Lost !! ");
    tone(buzzer, 350);
    for (int i = 0; i < 5; i++) digitalWrite(leds[i], HIGH);
    delay(1000);

    noTone(buzzer);
    delay(500);

    if (level - 1 > highScore) {
      highScore = level - 1;
    }

    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print("Score: "); 
    lcd.print(level - 1);

    lcd.setCursor(0, 1); 
    lcd.print("HighScore: ");
    lcd.print(highScore);

    victoryBuzzer();
    delay(2000);

    for (int i = 0; i < 5; i++) digitalWrite(leds[i], LOW);
    level = 1; stage = 0; lost = 0;
    lcd.clear();
    break;

  case 6:
    lcd.setCursor(0, 1); 
    lcd.print(" ** You  Win ** ");
    delay(1000);
    if (level == levelsInGame) {
      noTone(buzzer);
      lcd.setCursor(0, 0); lcd.print("Congratulation"); 
      lcd.setCursor(0, 1); lcd.print(" Level Complete");
      delay(1000); lcd.clear(); level = 1;          
    } else {
      level++;
    }
    stage = 1;
    break;

  default:
    break;
  }
}

void playBuzzer(int x) {
  tone(buzzer, 650 + (x * 100));
  delay(300);
  noTone(buzzer);
}

//สำหรับเพลงเมื่อจบเกม
void victoryBuzzer(){
  int melody[] = {
  1047, 1047, 1047, 1047, 831, 932, 1047, 932, 1047 //โน้ต
  };

  int noteDurations[] = {
  150, 150, 150, 400, 400, 400, 280, 150, 1000 //ระยะเวลาของแต่ละโน้ต
  };

  for (int i = 0; i < 9; i++) {
    tone(buzzer, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.0);
  }

  noTone(buzzer);
}
