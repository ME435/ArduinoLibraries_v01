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
char script1[] = "SCRIPT1";  // Set to a script name in your Android app
byte rxBuf[255];
volatile int mainEventFlags = 0;
#define FLAG_INTERRUPT_0 0x01

void setup() {
  Serial.begin(115200);
  armServos.attach();
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
  Serial.println();
  Serial.print("Position command: ");
  Serial.print(joint1Angle);
  Serial.print(" ");
  Serial.print(joint2Angle);
  Serial.print(" ");
  Serial.print(joint3Angle);
  Serial.print(" ");
  Serial.print(joint4Angle);
  Serial.print(" ");
  Serial.print(joint5Angle);
  Serial.println();
}

void jointAngleCallback(byte jointNumber, int jointAngle) {
  armServos.setJointAngle(jointNumber, jointAngle);
  Serial.println();
  Serial.print("Joint angle command for joint ");
  Serial.print(jointNumber);
  Serial.print(" to move to ");
  Serial.print(jointAngle);
  Serial.println();
}

void gripperCallback(int gripperDistance) {
  armServos.setGripperDistance(gripperDistance);
  Serial.println();
  Serial.print("Gripper command to ");
  Serial.print(gripperDistance);
  Serial.println();
}   

void loop() {
  if (mainEventFlags & FLAG_INTERRUPT_0) {
    delay(20);
    mainEventFlags &= ~FLAG_INTERRUPT_0;
    if (!digitalRead(2) && acc.isConnected()) {
      acc.write(script1, 6);
    }
  }
  // See if there is a new message from Android.
  if (acc.isConnected()) {
    int len = acc.read(rxBuf, sizeof(rxBuf), 1);
    if (len > 0) {
      // Print the message from Android to the PC monitor
      for (int x = 0; x < len - 1; x++) {
        Serial.print((char) rxBuf[x]);
      }
      robotCom.handleRxBytes(rxBuf, len);
    }
  }
}

