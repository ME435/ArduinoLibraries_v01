#include <WirelessThumperCom.h>

#define REVERSE 0
#define BRAKE   1
#define FORWARD 2
#define TEAM_NUMBER 4
#define CENTER_DEADBAND 5
#define CHANGE_THRESHOLD 5

WirelessThumperCom wtc(TEAM_NUMBER);
int lastSentVertReading = 0;
int lastSentHorzReading = 0;
byte leftMode, rightMode, leftDutyCycle, rightDutyCycle;
int initialHorzReading, initialVertReading;

void setup() {
  Serial.begin(9600);
  initialHorzReading = analogRead(0) - 512;
  initialVertReading = analogRead(1) - 512;
  //Serial.println(initialHorzReading);
  //Serial.println(initialVertReading);
}

void loop() {
  // Read both analog sensors
  int horzReading = analogRead(0) - 512 - initialHorzReading;
  int vertReading = analogRead(1) - 512 - initialVertReading;
  // Make near center readings exactly center
  if (abs(vertReading) < CENTER_DEADBAND) {
    vertReading = 0;
  }
  if (abs(horzReading) < CENTER_DEADBAND) {
    horzReading = 0;
  }
  // Determine if the change is worth mentioning to the thumper.
  if (abs(vertReading - lastSentVertReading) > CHANGE_THRESHOLD ||
      abs(horzReading - lastSentHorzReading) > CHANGE_THRESHOLD) {
      lastSentVertReading = vertReading;
      lastSentHorzReading = horzReading;
      
//      Serial.println();
//      Serial.print("VERT = ");
//      Serial.print(vertReading);
//      Serial.print("   HORZ = ");
//      Serial.print(horzReading);
      
      int leftMotor = (vertReading - horzReading) / 2;
      int rightMotor = (vertReading + horzReading) / 2;
      if (abs(leftMotor) < CENTER_DEADBAND) {
        leftMode = BRAKE;
      } else if (leftMotor > 0) {
        leftMode = FORWARD;
      } else {
        leftMode = REVERSE;
      }
      if (abs(rightMotor) < CENTER_DEADBAND) {
        rightMode = BRAKE;
      } else if (rightMotor > 0) {
        rightMode = FORWARD;
      } else {
        rightMode = REVERSE;
      }
      leftDutyCycle = min(abs(leftMotor), 255);
      rightDutyCycle = min(abs(rightMotor), 255);
      
//      Serial.println();
//      Serial.print("Sending ");
//      switch (leftMode) {
//        case REVERSE:
//          Serial.print("Left Reverse ");
//          break;
//        case BRAKE:
//          Serial.print("Left Brake ");
//          break;
//        case FORWARD:
//          Serial.print("Left Forward ");
//          break;
//      }
//      Serial.print(leftDutyCycle);
//      switch (rightMode) {
//        case REVERSE:
//          Serial.print(" Right Reverse ");
//          break;
//        case BRAKE:
//          Serial.print(" Right Brake ");
//          break;
//        case FORWARD:
//          Serial.print(" Right Forward ");
//          break;
//      }
//      Serial.println(rightDutyCycle);

      wtc.sendWheelSpeed(leftMode, rightMode, leftDutyCycle, rightDutyCycle);
  }
  delay(100);  
}
