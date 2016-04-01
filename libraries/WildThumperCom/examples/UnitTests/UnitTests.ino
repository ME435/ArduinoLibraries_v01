#include <WildThumperCom.h>

#define REVERSE 0
#define BRAKE   1
#define FORWARD 2
#define TEAM_NUMBER 16

WildThumperCom wtc(TEAM_NUMBER);

void setup() {
  Serial.begin(9600);  
  wtc.registerWheelSpeedCallback(updateWheelSpeed);
  runTests();
}

void runTests() {
  Serial.println("Test 1: ");
  Serial.print(" Expected : ");
  Serial.println("Update wheel speeds to Left Reverse 100 Right Reverse 100");
  Serial.print(" Actual   : ");
  test1();
  Serial.println("Test 2: ");
  Serial.print(" Expected : ");
  Serial.println("");
  Serial.print(" Actual   : ");
  test2();
  Serial.println(""); // necessary since nothing prints in test 2.
  Serial.println("Test 3: ");
  Serial.print(" Expected : ");
  Serial.println("Update wheel speeds to Left Reverse 99 Right Reverse 100");
  Serial.print(" Actual   : ");
  test3();
  Serial.println("Test 4: ");
  Serial.print(" Expected : ");
  Serial.println("Update wheel speeds to Left Brake 100 Right Forward 100");
  Serial.print(" Actual   : ");
  test4();
  Serial.println("Test 5: ");
  Serial.print(" Expected : ");
  Serial.println("Update wheel speeds to Left Reverse 126 Right Reverse 125");
  Serial.print(" Actual   : ");
  test5();
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
  Serial.println(rightDutyCycle);
}

void loop() {

}

/** Send all bytes received to the Wild Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    wtc.handleRxByte(Serial.read());
  }
}

void test1() {
  wtc.handleRxByte(START_BYTE);
  wtc.handleRxByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.handleRxByte(TEAM_NUMBER);
  wtc.handleRxByte(COMMAND_WHEEL_SPEED);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(100);
  wtc.handleRxByte(100);
  // Manually calculate crc 16+1+0+0+100+100 = 217
  wtc.handleRxByte(-217);
}

void test2() {
  wtc.handleRxByte(START_BYTE);
  wtc.handleRxByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.handleRxByte(TEAM_NUMBER);
  wtc.handleRxByte(COMMAND_WHEEL_SPEED);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(100);
  wtc.handleRxByte(100);
  // Manually calculate crc 16+1+0+0+100+100 = 217
  wtc.handleRxByte(-216);  // Intentially send the wrong CRC.
}

void test3() {
  wtc.handleRxByte(START_BYTE);
  wtc.handleRxByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.handleRxByte(TEAM_NUMBER);
  wtc.handleRxByte(COMMAND_WHEEL_SPEED);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(99);
  wtc.handleRxByte(100);
  // Manually calculate crc 16+1+0+0+99+100 = 216
  wtc.handleRxByte(-216);
}

void test4() {
  wtc.handleRxByte(START_BYTE);
  wtc.handleRxByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.handleRxByte(TEAM_NUMBER);
  wtc.handleRxByte(COMMAND_WHEEL_SPEED);
  wtc.handleRxByte(BRAKE);
  wtc.handleRxByte(FORWARD);
  wtc.handleRxByte(100);
  wtc.handleRxByte(100);
  // Manually calculate crc 16+1+1+2+100+100 = 220
  wtc.handleRxByte(-220);
}

void test5() {
  wtc.handleRxByte(START_BYTE);
  wtc.handleRxByte(WHEEL_SPEED_MESSAGE_LENGTH);
  wtc.handleRxByte(TEAM_NUMBER);
  wtc.handleRxByte(COMMAND_WHEEL_SPEED);
  wtc.handleRxByte(REVERSE);
  wtc.handleRxByte(REVERSE);
  // Escaping manually the start byte and the escape byte
  wtc.handleRxByte(125);
  wtc.handleRxByte(94);
  wtc.handleRxByte(125);
  wtc.handleRxByte(93);
  // Manually calculate crc 16+1+0+0+126+125 = 12
  wtc.handleRxByte(-12);
}
