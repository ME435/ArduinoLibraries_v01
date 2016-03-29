/**
   ME435 Shield Template + ADK Connection
*/
#include <LiquidCrystal.h>
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

char manufacturer[] = "Rose-Hulman";
char model[] = "My App Name";
char versionStr[] = "1.0";

// Note, the second model parameter is really the description.
// Can be a handy hacky trick to state the model AS the description though.
AndroidAccessory acc(manufacturer, model, model, versionStr,
                     "https://sites.google.com/site/me435spring2016/", "12345");
char rxBuf[255];

/***  Pin I/O   ***/
#define PIN_LED_1 64
#define PIN_LED_2 65
#define PIN_LED_3 66
#define PIN_LED_4 67
#define PIN_LED_5 68
#define PIN_LED_6 69
#define PIN_RIGHT_BUTTON 2
#define PIN_LEFT_BUTTON 3
#define PIN_SELECT_BUTTON 21
#define PIN_CONTRAST_ANALOG 8
#define PIN_HORZ_ANALOG 0
#define PIN_VERT_ANALOG 1

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01
#define FLAG_INTERRUPT_1 0x02
#define FLAG_INTERRUPT_2 0x04

/***  LCD ***/
LiquidCrystal lcd(14, 15, 16, 17, 18, 19, 20);
#define LINE_1 0
#define LINE_2 1

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);
  pinMode(PIN_LED_5, OUTPUT);
  pinMode(PIN_LED_6, OUTPUT);
  digitalWrite(PIN_LED_1, LOW);
  digitalWrite(PIN_LED_2, LOW);
  digitalWrite(PIN_LED_3, LOW);
  digitalWrite(PIN_LED_4, LOW);
  digitalWrite(PIN_LED_5, LOW);
  digitalWrite(PIN_LED_6, LOW);
  pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(PIN_SELECT_BUTTON, INPUT_PULLUP);
  attachInterrupt(0, int0_isr, FALLING);
  attachInterrupt(1, int1_isr, FALLING);
  attachInterrupt(2, int2_isr, FALLING);
  lcd.begin(16, 2);
  delay(1500);
  acc.powerOn();
}

void loop() {
  int horzReading = analogRead(PIN_HORZ_ANALOG);
  int vertReading = analogRead(PIN_VERT_ANALOG);

  if (acc.isConnected()) {
    checkForInterrupts();
    checkForRxMessages();

    // Do stuff here.  The Android device is connected.
  }


  // Only do stuff here that doesn't need an Android device.
}

void checkForInterrupts() {
  if (mainEventFlags & FLAG_INTERRUPT_0) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_0;
    if (!digitalRead(PIN_RIGHT_BUTTON)) {
      // do stuff
    }
  }
  if (mainEventFlags & FLAG_INTERRUPT_1) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_1;
    if (!digitalRead(PIN_LEFT_BUTTON)) {
      // do stuff
    }
  }
  if (mainEventFlags & FLAG_INTERRUPT_2) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_2;
    if (!digitalRead(PIN_SELECT_BUTTON)) {
      // do stuff
    }
  }
}


void checkForRxMessages() {
  int len = acc.read(rxBuf, sizeof(rxBuf), 1);
  if (len > 0) {
    rxBuf[len - 1] = '\0';
    String inputString = String(rxBuf);

    // DO something with the message.
    if (inputString.equalsIgnoreCase("ON")) {
      // DO something.
    } else if (inputString.equalsIgnoreCase("OFF")) {
      // DO something.
    }
  }
}


// Simple ISRs that set flags only
void int0_isr() {
  mainEventFlags |= FLAG_INTERRUPT_0;
}

void int1_isr() {
  mainEventFlags |= FLAG_INTERRUPT_1;
}

void int2_isr() {
  mainEventFlags |= FLAG_INTERRUPT_2;
}

