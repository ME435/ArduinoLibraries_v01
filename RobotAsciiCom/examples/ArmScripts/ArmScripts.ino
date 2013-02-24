#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <Servo.h>
#include <ArmServos.h>
#include <RobotAsciiCom.h>

// Only Manufacturer, Model, and Version matter to Android
AndroidAccessory acc("Rose-Hulman",
                     "Arm Scripts",
                     "Basic command set to carry out Arm Scripts",
                     "1.0",
                     "https://sites.google.com/site/me435spring2013/",
                     "12345");

// Create the arm and command parser libraries.
ArmServos armServos;
RobotAsciiCom robotCom;

// Global variables.
char script1[] = "SIMPLE";  // Set to a script name in your Android app
char script2[] = "S2";  // Set to a another script name in your Android app
char rxBuf[255];
byte ledIsOn = 0;
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01

void setup() {
  Serial.begin(115200);
  armServos.attach();
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);    
  attachInterrupt(0, int0_isr, FALLING);
  robotCom.registerPositionCallback(positionCallback);
  robotCom.registerJointAngleCallback(jointAngleCallback);
  robotCom.registerGripperCallback(gripperCallback); 
  delay(1500);
  acc.powerOn();
}

void int0_isr() {
  mainEventFlags |= FLAG_INTERRUPT_0;
}

void positionCallback(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
  armServos.setPosition(joint1Angle, joint2Angle, joint3Angle, joint4Angle, joint5Angle);
}

void jointAngleCallback(byte jointNumber, int jointAngle) {
  armServos.setJointAngle(jointNumber, jointAngle);
}

void gripperCallback(int gripperDistance) {
  armServos.setGripperDistance(gripperDistance);
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
      // Print the message from Android to the PC monitor
//      for (int x = 0; x < len; x++) {
//        Serial.print((char) rxBuf[x]);
//      }
	  robotCom.handleRxBytes(rxBuf, len);
    }
  }
}

