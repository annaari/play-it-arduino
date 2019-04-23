#include <Adafruit_NeoPixel.h>
#include <string.h>

#define LED_PIN    2
#define LED_PER_FRET 6
#define FRET_COUNT 4
#define LED_COUNT (FRET_COUNT+1) * LED_PER_FRET

int commandLength;
int index;
int pixelIndex;
int charOffset = '0';
int currentFret;
int currentString;
int fixedStringNumber;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t colorOff = strip.Color(0,0,0);
uint32_t colorR = strip.Color(255,0,0);
uint32_t colorG = strip.Color(0,255,0);
uint32_t colorB = strip.Color(0,0,255);
uint32_t colorY = strip.Color(255,255,0);
uint32_t colorW = strip.Color(255,255,255);
uint32_t pixelColor;

void setup() {
  Serial.begin(9600);
  strip.begin();
  clearPixels();
  strip.show();
  strip.setBrightness(2);
}

// Command format: repeating sequences of FSC
//  - F = Fret number, from 0 to FRET_COUNT
//  - S = String number, from 1 to LED_PER_FRET
//  - C = Color code, from A to G
// For example: "01W22G23B14R05W06W" for E major.

void loop() {
  if (Serial.available() > 0) {
    processCommand(Serial.readString());
    Serial.flush();
  }
}

void processCommand(String command) {
  Serial.print("I received: ");
  Serial.println(command);
  commandLength = command.length();
  if (commandLength > 0) {
    index = 0;
    clearPixels();
    while(index <= commandLength - 3) {
      processPixel(command[index], command[index+1], command[index+2]);
      index = index+3;
    }
    strip.show();
  }
}

void processPixel(char currentFretCode, char currentStringCode, char currentColorCode) {
  currentFret =  currentFretCode - charOffset;
  currentString = currentStringCode - charOffset;
  Serial.print("Turning LED under string ");
  Serial.print(currentString);
  Serial.print(" on fret ");
  Serial.println(currentFret);
  if (currentFret % 2 == 0) {
    fixedStringNumber = currentString - 1;
  } else {
    fixedStringNumber = LED_PER_FRET - currentString;
  }
  pixelIndex = (FRET_COUNT - currentFret) * LED_PER_FRET + fixedStringNumber;
  switch(currentColorCode) {
    case 'R':
      pixelColor = colorR;
      break;
    case 'G':
      pixelColor = colorG;
      break;
    case 'B':
      pixelColor = colorB;
      break;
    case 'Y':
      pixelColor = colorY;
      break;
    case 'W':
      pixelColor = colorW;
      break;
    default:
      pixelColor = colorOff;
  }
  singlePixel(pixelIndex, pixelColor);
}

void clearPixels() {
  Serial.println("Clearing pixels");
  for (int i = 0; i <= strip.numPixels(); i++) { 
    strip.setPixelColor(i, colorOff);
  }
}

void singlePixel(int index, uint32_t color) {
  strip.setPixelColor(index, color);
}
