#include <DIYables_4Digit7Segment_74HC595.h>
#define IDLE 0
#define SHOW 1
#define PLAY 2
#define RESET 3
int stateGame = IDLE;
const int rLed = 2, yLed = 3, gLed = 4, bLed = 12;
const int rBtn = 8, yBtn = 9, gBtn = 10, bBtn = 11;
const int ledPins[4] = { rLed, yLed, gLed, bLed };
const int buttonPins[4] = { rBtn, yBtn, gBtn, bBtn };
int clk = 5, ltc = 6, dta = 7;
DIYables_4Digit7Segment_74HC595 display(clk, ltc, dta);
int idleCurrentLed = 0;
int level = 1;
int currentStep = 0;
int sequence [20];
int showIndex = 0;
bool isLedOn = false;
unsigned long idleLastTime = 0;
unsigned long showLastTime = 0;
unsigned long showDelay = 600;



void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  randomSeed(analogRead(A1));
}


//פונקציית הדלקת הלדים לפני התחלת המשחק. רצה עד שהשחקן לוחץ על אחד הכפתורים
void idleState(){
  unsigned long now = millis();

  display.printInt(0, false);
  display.loop();

  if (now - idleLastTime > 300){
    clearLeds();
    int nextLed;
    do {
      nextLed = random(4);
    } 
    while (nextLed == idleCurrentLed);

    idleCurrentLed = nextLed;

    digitalWrite(ledPins[idleCurrentLed], HIGH);
    idleLastTime = now;
  }
  for (int i = 0; i < 4; i++){
    if (digitalRead(buttonPins[i]) == LOW){
      clearLeds();
      delay (500);
      stateGame = SHOW;
      level = 1;
      currentStep = 0;
      generateSequence();
      break;
    }
  }
}

//פונקציית יצירת רצף הצבעים למשחק
void generateSequence(){
  if (level <= 20){
    sequence [level - 1] = random(4);
  }
}

//פונקציית הדלקת הלדים לפי השלב שבו נמצא השחקן
void showSequence(){
  display.printInt(level, false);
  display.loop();

  unsigned long now = millis();
  showDelay = 600 - ((level - 1) / 5) * 100;

  if (showDelay < 200) showDelay = 200;

  if (isLedOn && now - showLastTime >= showDelay){
    digitalWrite(ledPins[sequence[showIndex]], LOW);
    isLedOn = false;
    showLastTime = now;
    showIndex++;
  }
  if (!isLedOn && showIndex < level && now - showLastTime >= 200){
    digitalWrite(ledPins[sequence[showIndex]], HIGH);
    isLedOn = true;
    showLastTime = now;
  }
  if (showIndex >= level){
    clearLeds();
    showIndex = 0;
    currentStep = 0;
    isLedOn = false;
    stateGame = PLAY;
    delay (100);
  }
}


//פונקציית בדיקת הלחיצות של השחקן
void playState(){
  display.printInt(level, false);
  display.loop();

  for (int i = 0; i < 4; i++){
    if (digitalRead(buttonPins[i]) == LOW){
      while (digitalRead(buttonPins[i]) == LOW);
      delay (50);
      if (i == sequence[currentStep]){
        currentStep++;
        if (currentStep >= level){
          level++;
          if (level > 20) level = 20;
          generateSequence();
          stateGame = SHOW;
          delay (500);
        }
      }
      else {
        stateGame = RESET;
        delay (500);
      }
      break;
    }
  }
}


//פונקציית איפוס המשחק (במידה והשחקן טעה)
void resetGame(){
  clearLeds();
  level = 1;
  currentStep = 0;
  showIndex = 0;
  idleCurrentLed = 0;
  stateGame = IDLE;
  delay (500);
}


//פונקציית כיבוי נורות
void clearLeds(){
  for (int i = 0; i < 4; i++){
    digitalWrite(ledPins[i], LOW);
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  switch (stateGame) {
    case IDLE:
      idleState();
      break;
    case SHOW:
      showSequence();
      break;
    case PLAY:
      playState();
      break;
    case RESET:
      resetGame();
      break;
  }
}
