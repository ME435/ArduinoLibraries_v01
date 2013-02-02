#include <WirelessThumperCom.h>

#define REVERSE 0
#define BRAKE   1
#define FORWARD 2
#define TEAM_NUMBER 16

WirelessThumperCom wtc(TEAM_NUMBER);

void setup() {
  Serial.begin(9600);
}

void updateWheelSpeed(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  Serial.print("Update wheel speeds to ");
  switch (leftMode) {
    case REVERSE:
      Serial.print("Left Reverse ");
      break;
    case BRAKE:
      Serial.print("Left Brake ");
      break;
    case FORWARD:
      Serial.print("Left Forward ");
      break;
  }
  Serial.print(leftDutyCycle);
  switch (rightMode) {
    case REVERSE:
      Serial.print(" Right Reverse ");
      break;
    case BRAKE:
      Serial.print(" Right Brake ");
      break;
    case FORWARD:
      Serial.print(" Right Forward ");
      break;
  }
  Serial.print(rightDutyCycle);
}

void loop() {
  wtc.registerWheelSpeedCallback(updateWheelSpeed);
  Serial.print("Test 1: ");
  test1();
  Serial.println("");
  Serial.print("Test 2: ");
  test2();
  Serial.println("");
  Serial.print("Test 3: ");
  test3();
  Serial.println("");
  Serial.print("Test 4: ");
  test4();
  Serial.println("");
  Serial.print("Test 5: ");
  test5();
  Serial.println("");
  // Output should be...
  //Test 1: Update wheel speeds to Left Reverse 100 Right Reverse 100
  //Test 2: 
  //Test 3: Update wheel speeds to Left Reverse 100 Right Reverse 100
  //Test 4: Update wheel speeds to Left Brake 99 Right Forward 100
  //Test 5: Update wheel speeds to Left Reverse 126 Right Reverse 125

  while(1);
}

void test1() {
  wtc.parseByte(START_BYTE);
  wtc.parseByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.parseByte(TEAM_NUMBER);
  wtc.parseByte(COMMAND_WHEEL_SPEED);
  wtc.parseByte(REVERSE);
  wtc.parseByte(REVERSE);
  wtc.parseByte(100);
  wtc.parseByte(100);
  // Manually calculate crc 16+1+0+0+100+100 = 217
  wtc.parseByte(-217);
}

void test2() {
  wtc.parseByte(START_BYTE);
  wtc.parseByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.parseByte(TEAM_NUMBER);
  wtc.parseByte(COMMAND_WHEEL_SPEED);
  wtc.parseByte(REVERSE);
  wtc.parseByte(REVERSE);
  wtc.parseByte(100);
  wtc.parseByte(100);
  // Manually calculate crc 16+1+0+0+100+100 = 217
  wtc.parseByte(-216);
}

void test3() {
  wtc.parseByte(START_BYTE);
  wtc.parseByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.parseByte(TEAM_NUMBER);
  wtc.parseByte(COMMAND_WHEEL_SPEED);
  wtc.parseByte(REVERSE);
  wtc.parseByte(REVERSE);
  wtc.parseByte(99);
  wtc.parseByte(100);
  // Manually calculate crc 16+1+0+0+99+100 = 216
  wtc.parseByte(-216);
}

void test4() {
  wtc.parseByte(START_BYTE);
  wtc.parseByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.parseByte(TEAM_NUMBER);
  wtc.parseByte(COMMAND_WHEEL_SPEED);
  wtc.parseByte(BRAKE);
  wtc.parseByte(FORWARD);
  wtc.parseByte(100);
  wtc.parseByte(100);
  // Manually calculate crc 16+1+1+2+100+100 = 220
  wtc.parseByte(-220);
}

void test5() {
  wtc.parseByte(START_BYTE);
  wtc.parseByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.parseByte(TEAM_NUMBER);
  wtc.parseByte(COMMAND_WHEEL_SPEED);
  wtc.parseByte(REVERSE);
  wtc.parseByte(REVERSE);
  // Escaping manually the start byte and the escape byte
  wtc.parseByte(125);
  wtc.parseByte(94);
  wtc.parseByte(125);
  wtc.parseByte(93);
  // Manually calculate crc 16+1+0+0+126+125 = 12
  wtc.parseByte(-12);
}
