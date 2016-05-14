/**
 * This program should provide all functions needed on the base robot.
 * 
 * This particular version is a port of the PreInstalledWildThumperProgram.
 * Wild Thumper boards are blue.
 * Trex boards are yellow.
 */

// To connect serial communication lines
// D0 - Ground is on the outside, 5v power in the middle, Thumper Rx on inside (connect this to ADK Tx)
// D1 - Ground is on the outside, 5v power in the middle, Thumper Tx on inside (connect this to ADK Rx)

/*
Changes made to transition from Wild Thumper to T Rex:
  -program as an arduino nano
  -equation changes for calculating battery voltage and motor current.
  -motor max current raise to 18 amp
  -battery voltage check will only run when no motors are running
  -change in motor command structure and pinout
      --pins for pwm, dir, brake instead of manually setting two channels for each motor
  -removed charging pin
  -PIN_TEAM_CHANGE_ENABLE is now D13
  -changed servo pins. now it goes (inside, D13))J1, J2, J3, J4, J5, Gr (outside, D5)
*/


#include <WildThumperCom.h>
#include <Servo.h>
#include <ArmServos.h>
#include <ArmServosSpeedControlled.h>
#include <EEPROM.h>

#define DEFAULT_TEAM_NUMBER 4  // Used initially as a placeholder
// Only used as the actual team number if EEPROM is empty.

// ==================== IO Pins =========================
//#define PIN_LEFT_MOTOR_CH_A        3  // Left  motor H bridge, input A
//#define PIN_LEFT_MOTOR_CH_B       11  // Left  motor H bridge, input B
//#define PIN_RIGHT_MOTOR_CH_A       5  // Right motor H bridge, input A
//#define PIN_RIGHT_MOTOR_CH_B       6  // Right motor H bridge, input B

#define PIN_LEFT_MOTOR_BRAKE      4
#define PIN_LEFT_MOTOR_DIR        2
#define PIN_LEFT_MOTOR_PWM        3

#define PIN_RIGHT_MOTOR_BRAKE     9
#define PIN_RIGHT_MOTOR_DIR      10
#define PIN_RIGHT_MOTOR_PWM      11

#define PIN_BATTERY                3  // Analog input 03
#define PIN_RIGHT_MOTOR_CURRENT    7  // Analog input 07
#define PIN_LEFT_MOTOR_CURRENT     6  // Analog input 06
//#define PIN_CHARGER               13  // Low=ON High=OFF
#define CHARGER_OFF                1  // Make sure charger is off (not useful)
//#define PIN_TEAM_CHANGE_ENABLE    15  // Using Analog input 1 as a digital pin - Not available on Trex board
#define PIN_TEAM_CHANGE_ENABLE    13  // Using digital 13 - Not actually needed since we are ignoring team number
#define PIN_JOINT_1                13  // Arm servo 
#define PIN_JOINT_2                12  // Arm servo
#define PIN_JOINT_3                8  // Arm servo
#define PIN_JOINT_4                7  // Arm servo
#define PIN_JOINT_5               6  // Arm servo
#define PIN_GRIPPER               5  // Arm servo

// =============== H BRIDGE SETTINGS ====================
#define BATTERY_SHUTDOWN_MILLIVOLTS     6400     // This is the millivolt reading at which the battery is too low to continue. (6.4 volts)
#define MOTOR_SHUTDOWN_MILLIAMPS        18000     // overload current limit for motor (help reduce the risk of stalled motors drawing too much current) (9 amps)
#define COOLDOWN_TIME        100     // time in mS before motor is re-enabled after overload occurs

//wheel modes?
#define WHEEL_SPEED_MODE_REVERSE   0
#define WHEEL_SPEED_MODE_BRAKE     1
#define WHEEL_SPEED_MODE_FORWARD   2

WildThumperCom wtc(DEFAULT_TEAM_NUMBER);  // Team number saved in EEPROM will load in setup.
unsigned long lastLeftMotorCurrentOverloadTime_ms = 0;
unsigned long lastRightMotorCurrentOverloadTime_ms = 0;
int leftMotorMode = WHEEL_SPEED_MODE_BRAKE;                          // 0=reverse, 1=brake, 2=forward
int rightMotorMode = WHEEL_SPEED_MODE_BRAKE;                         // 0=reverse, 1=brake, 2=forward
int leftMotorDutyCycle;                                              // PWM value for left  motor speed / brake
int rightMotorDutyCycle;                                             // PWM value for right motor speed / brake
int batteryInMillivolts;       // Battery voltage in millivolts.
int leftCurrentInMilliamps;    // Left motor current draw in milliamps.
int rightCurrentInMilliamps;   // Right motor current draw in milliamps.


ArmServosSpeedControlled armServos(PIN_JOINT_1, PIN_JOINT_2, PIN_JOINT_3, PIN_JOINT_4, PIN_JOINT_5, PIN_GRIPPER);

/*** Interrupt flags ***/
volatile int mainEventFlags = 0;
#define FLAG_SEND_BATTERY_VOLTAGE 0x01
#define FLAG_SEND_WHEEL_CURRENT   0x02
#define FLAG_SEND_TEAM_NUMBER     0x04

void setup() {
  // Make sure the charger is always off (not useful and dangerous for our LiPo batteries)
  //pinMode(PIN_CHARGER, OUTPUT);
  
  pinMode(PIN_LEFT_MOTOR_BRAKE, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_DIR, OUTPUT);
  pinMode(PIN_LEFT_MOTOR_PWM, OUTPUT);
  
  pinMode(PIN_RIGHT_MOTOR_BRAKE, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(PIN_RIGHT_MOTOR_PWM, OUTPUT);
  
  //digitalWrite (PIN_CHARGER, CHARGER_OFF);
  
  // Setup the team change enable pin as an input with pullup.
  // This pin must be held low to allow the team number to change.
  // If the pin is high, requests to change the team number are ignored.
  pinMode(PIN_TEAM_CHANGE_ENABLE, INPUT_PULLUP);

  Serial.begin(9600);
  wtc.registerWheelSpeedCallback(wheelSpeedCallback);
  wtc.registerPositionCallback(positionCallback);
  wtc.registerJointAngleCallback(jointAngleCallback);
  wtc.registerGripperCallback(gripperDistanceCallback);
  wtc.registerBatteryVoltageRequestCallback(batteryVoltageRequestCallback);
  wtc.registerWheelCurrentRequestCallback(wheelCurrentRequestCallback);
  wtc.registerTeamNumberRequestCallback(teamNumberRequestCallback);
  wtc.registerTeamNumberChangeRequestCallback(teamNumberChangeRequestCallback);
  wtc.registerAttachSelectedServosCallback(attachSelectedServosCallback);
  wtc.setTeamNumber(readEepromTeamNumber());
  // Defaulting to having the servo arm detached.  Must send attach command to use arm.
  // armServos.attach();
}

/** Handle a request to update the wheel speed variables based on the new values received. */
void wheelSpeedCallback(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  leftMotorMode = leftMode;
  leftMotorDutyCycle = leftDutyCycle;
  rightMotorMode = rightMode;
  rightMotorDutyCycle = rightDutyCycle; 
}

/** Handle a request to move the arm to a set position (joints 1-5 only). */
void positionCallback(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
  armServos.setPosition(joint1Angle, joint2Angle, joint3Angle, joint4Angle, joint5Angle);
}

/** Handle a request to move a single joint to an angle (does not use speed control to get there). */
void jointAngleCallback(byte jointNumber, int jointAngle) {
  armServos.setJointAngle(jointNumber, jointAngle);
}

/** Handle a request to open / close the gripper. */
void gripperDistanceCallback(int gripperDistance) {
  armServos.setGripperDistance(gripperDistance);
}

/** Handle a request to send back the battery voltage. */
void batteryVoltageRequestCallback() {
  mainEventFlags |= FLAG_SEND_BATTERY_VOLTAGE;
}

/** Handle a request to send back the wheel current levels. */
void wheelCurrentRequestCallback() {
  mainEventFlags |= FLAG_SEND_WHEEL_CURRENT;
}

/** Handle a request to send back the team number. */
void teamNumberRequestCallback() {
  mainEventFlags |= FLAG_SEND_TEAM_NUMBER;
}

/** Handle a request to change the team number (team enable pin must be held low). */
void teamNumberChangeRequestCallback(byte teamNumber) {
  if(digitalRead(PIN_TEAM_CHANGE_ENABLE)) {
    // Pin is high.  Do NOT change the team number.
  } else {
    // Pin is low.  Change to the new team number and save setting.
    wtc.setTeamNumber(teamNumber);
    writeEepromTeamNumber(teamNumber);
    Serial.println(teamNumber);
  }
  mainEventFlags |= FLAG_SEND_TEAM_NUMBER; // Always reply with team number.
}

/** Handle a request to attach certain servos. See ArmServos.h for parameter values. */
void attachSelectedServosCallback(byte servosToEnable) {
  armServos.attachSelectedServos(servosToEnable);
}

void loop() {
  armServos.updateServos();
  if (mainEventFlags & FLAG_SEND_TEAM_NUMBER) {
    mainEventFlags &= ~FLAG_SEND_TEAM_NUMBER;
    wtc.sendTeamNumberReply(readEepromTeamNumber());
  }
  
  //--------------------- Check battery voltage ---------------------
  int batteryVoltageAnalogReading = analogRead(PIN_BATTERY);      // read the battery voltage
  
  //130=5v
  //257 = 7.8V
  batteryInMillivolts = batteryVoltageAnalogReading*30;
//  batteryInMillivolts = batteryVoltageAnalogReading * 15 - batteryVoltageAnalogReading / 3; // 5 / 1023 * 3 * 1000
  if (mainEventFlags & FLAG_SEND_BATTERY_VOLTAGE) {
    mainEventFlags &= ~FLAG_SEND_BATTERY_VOLTAGE;
    wtc.sendBatteryVoltageReply(batteryInMillivolts);
  }
  if (batteryInMillivolts < BATTERY_SHUTDOWN_MILLIVOLTS && (leftMotorDutyCycle==0 && rightMotorDutyCycle==0)) {
  //Serial.println((int)batteryInMillivolts);    
    // Shut down the Wild Thumper if the battery voltage is too low.
    //Serial.println("voltage issues");
    //Serial.println((int)batteryInMillivolts);
    if ((millis() / 1000) % 2) {
      // This makes a beeping noise without moving the robot.
      wheelSpeedCallback(WHEEL_SPEED_MODE_FORWARD, WHEEL_SPEED_MODE_FORWARD, 20, 20);
    } else {
      wheelSpeedCallback(WHEEL_SPEED_MODE_BRAKE, WHEEL_SPEED_MODE_BRAKE, 0, 0);
    }
  }

  //---------------- Check current draw of motors --------------------
  int leftCurrentAnalogReading = analogRead(PIN_LEFT_MOTOR_CURRENT);     // read left motor current draw
  int rightCurrentAnalogReading = analogRead(PIN_RIGHT_MOTOR_CURRENT);   // read right motor current draw
//  leftCurrentInMilliamps = leftCurrentAnalogReading * 20;     // left motor current draw in milliamps
//  rightCurrentInMilliamps = rightCurrentAnalogReading * 20;   // right motor current draw in milliamps
  leftCurrentInMilliamps = (leftCurrentAnalogReading-511)*48.83;     // left motor current draw in milliamps
  rightCurrentInMilliamps = (rightCurrentAnalogReading-511)*48.83;   // right motor current draw in milliamps
  if (mainEventFlags & FLAG_SEND_WHEEL_CURRENT) {
    mainEventFlags &= ~FLAG_SEND_WHEEL_CURRENT;
    wtc.sendWheelCurrentReply(leftCurrentInMilliamps, rightCurrentInMilliamps);
  }
  if (leftCurrentInMilliamps > MOTOR_SHUTDOWN_MILLIAMPS)            // is motor current draw exceeding safe limit
  {
//    analogWrite(PIN_LEFT_MOTOR_CH_A, 0);                            // turn off motors
//    analogWrite(PIN_LEFT_MOTOR_CH_B, 0);                            // turn off motors
      analogWrite(PIN_LEFT_MOTOR_PWM, 0);
    lastLeftMotorCurrentOverloadTime_ms = millis();        // record time of overload
    //Serial.println("current issues");
  }
  if (rightCurrentInMilliamps > MOTOR_SHUTDOWN_MILLIAMPS)           // is motor current draw exceeding safe limit
  {
//    analogWrite(PIN_RIGHT_MOTOR_CH_A, 0);                           // turn off motors
//    analogWrite(PIN_RIGHT_MOTOR_CH_B, 0);                           // turn off motors
      analogWrite(PIN_RIGHT_MOTOR_PWM, 0);
    lastRightMotorCurrentOverloadTime_ms = millis();                // record time of overload
  }
  
  //--------------------- Set the motor speeds (if not in a stalled motor state) ---------------------
  if ((millis()-lastLeftMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)             
  {
    switch (leftMotorMode)
    {
      case WHEEL_SPEED_MODE_FORWARD:
        //analogWrite(PIN_LEFT_MOTOR_CH_A, 0);
        //analogWrite(PIN_LEFT_MOTOR_CH_B, leftMotorDutyCycle);
        digitalWrite(PIN_LEFT_MOTOR_DIR, 0);
        analogWrite(PIN_LEFT_MOTOR_PWM, leftMotorDutyCycle);
        digitalWrite(PIN_LEFT_MOTOR_BRAKE, 0);
        break;
      case WHEEL_SPEED_MODE_BRAKE:
        //analogWrite(PIN_LEFT_MOTOR_CH_A, leftMotorDutyCycle);
        //analogWrite(PIN_LEFT_MOTOR_CH_B, leftMotorDutyCycle);
        digitalWrite(PIN_LEFT_MOTOR_BRAKE, 1);
        digitalWrite(PIN_LEFT_MOTOR_DIR, 0);
        analogWrite(PIN_LEFT_MOTOR_PWM, leftMotorDutyCycle);
        break;
      case WHEEL_SPEED_MODE_REVERSE:
        //analogWrite(PIN_LEFT_MOTOR_CH_A, leftMotorDutyCycle);
        //analogWrite(PIN_LEFT_MOTOR_CH_B, 0);
        digitalWrite(PIN_LEFT_MOTOR_DIR, 1);
        analogWrite(PIN_LEFT_MOTOR_PWM, leftMotorDutyCycle);
        digitalWrite(PIN_LEFT_MOTOR_BRAKE, 0);
        
        break;
    }
  }
  if ((millis()-lastRightMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)
  {
    switch (rightMotorMode)
    {
      case WHEEL_SPEED_MODE_FORWARD:
        //analogWrite(PIN_RIGHT_MOTOR_CH_A, 0);
        //analogWrite(PIN_RIGHT_MOTOR_CH_B, rightMotorDutyCycle);
        digitalWrite(PIN_RIGHT_MOTOR_DIR, 0);
        analogWrite(PIN_RIGHT_MOTOR_PWM, rightMotorDutyCycle);
        digitalWrite(PIN_RIGHT_MOTOR_BRAKE, 0);
        break;
      case WHEEL_SPEED_MODE_BRAKE:
//        analogWrite(PIN_RIGHT_MOTOR_CH_A, rightMotorDutyCycle);
//        analogWrite(PIN_RIGHT_MOTOR_CH_B, rightMotorDutyCycle);
        digitalWrite(PIN_RIGHT_MOTOR_DIR, 0);
        analogWrite(PIN_RIGHT_MOTOR_PWM, rightMotorDutyCycle);
        digitalWrite(PIN_RIGHT_MOTOR_BRAKE, 1);
        break;
      case WHEEL_SPEED_MODE_REVERSE:
//        analogWrite(PIN_RIGHT_MOTOR_CH_A, rightMotorDutyCycle);
//        analogWrite(PIN_RIGHT_MOTOR_CH_B, 0);
        digitalWrite(PIN_RIGHT_MOTOR_DIR, 1);
        analogWrite(PIN_RIGHT_MOTOR_PWM, rightMotorDutyCycle);
        digitalWrite(PIN_RIGHT_MOTOR_BRAKE, 0);
        break;
    }
  }
}

/** Send all bytes received to the Wild Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    wtc.handleRxByte(Serial.read());
  }
}


