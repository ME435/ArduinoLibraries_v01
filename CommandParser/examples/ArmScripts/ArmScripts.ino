#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <Servo.h>
#include <Arm.h>
#include <CommandParser.h>

// Only Manufacturer, Model, and Version matter to Android
AndroidAccessory acc("Rose-Hulman",
                     "Arm Scripts",
                     "Basic command set to carry out Arm Scripts",
                     "1.0",
                     "https://sites.google.com/site/me435spring2013/",
                     "12345");

// Create the arm and command parser libraries.
Arm robotArm;
CommandParser commandParser;

// Global variables.
char script1[] = "SIMPLE";  // Set to a script name in your Android app
char script2[] = "S2";  // Set to a another script name in your Android app
char rxBuf[255];
byte ledIsOn = 0;
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01

void setup() {
  Serial.begin(115200);
  robotArm.init();
  commandParser.init(robotArm);
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);    
  attachInterrupt(0, int0_isr, FALLING);
  delay(1500);
  acc.powerOn();
}

void int0_isr() {
  mainEventFlags |= FLAG_INTERRUPT_0;
}

void loop() {

    // Toggle the LED after an interrupt for debug feedback.
    if (mainEventFlags & FLAG_INTERRUPT_0) {
      //Serial.println("Received and interrupt!");
      delay(20);
      mainEventFlags &= !FLAG_INTERRUPT_0;
      // Only perform the action if the switch is still low.
      // This is a simple software debounce mechanism.
      if (!digitalRead(2)) {
        if (ledIsOn) {
          digitalWrite(13, LOW);
          ledIsOn = 0;
          if (acc.isConnected()) {
            acc.write(script1, 6);
            Serial.print("Sent ");
            Serial.println(script1);
          }
        } else {
          digitalWrite(13, HIGH);
          ledIsOn = 1;
          if (acc.isConnected()) {
            acc.write(script2, 2);
            Serial.println("Sent ");
            Serial.println(script1);
          }
        }
      }
    }

  // See if there is a new message from Android.
  if (acc.isConnected()) {
    int len = acc.read(rxBuf, sizeof(rxBuf), 1);
    if (len > 0) {
      
      // Toggle the LED every message just for fun.
//      if (ledIsOn) {
//        digitalWrite(13, LOW);
//        ledIsOn = 0;
//      } else {
//        digitalWrite(13, HIGH);
//        ledIsOn = 1;
//      }
      
      // Print the message from Android to the PC monitor
//      for (int x = 0; x < len; x++) {
//        Serial.print((char) rxBuf[x]);
//      }
      
      // Actually do something with the message.
      // All my messages should end with my terminator character (ASCII 13)
      if (rxBuf[len - 1] == '\n') {
        // Convert to a String and parse the message to figure out the details
        rxBuf[len - 1] = '\0';
        String rxStr = String(rxBuf);
        commandParser.parseCommand(rxStr);
      } 
    } // close len > 0
  } // close acc is connected
}  // close loop()

