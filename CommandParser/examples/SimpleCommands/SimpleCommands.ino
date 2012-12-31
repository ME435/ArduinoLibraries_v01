#include <Servo.h>
#include <Arm.h>
#include <CommandParser.h>

Arm robotArm;
CommandParser commandParser;

void setup() {
  Serial.begin(115200);
  delay(100);
  robotArm.init();
  commandParser.init(robotArm);
  commandParser.parseCommand("GRIPPER 33");
  commandParser.parseCommand("POSITION 1 2 3 4 5");
  commandParser.parseCommand("JOINT 2 ANGLE 44");
}

void loop() {
}
