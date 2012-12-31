#include <Servo.h>

#include <Arm.h>

Arm robotArm;

void setup() {
  Serial.begin(115200);
  delay(100);
  robotArm.init();  
//  Serial.println(robotArm.getGripperDistance());
  robotArm.setGripperDistance(25);
  Serial.println(robotArm.getGripperDistance());

}

void loop() {
}
