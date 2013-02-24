// Note, for the Wild Thumper board make this change above.
//
// Tools -> Board -> Arduino Pro or Pro Mini (5v, 16MHz) w/ATmega168
//

/**
 * This program is an example of the code that would run on the Wild Thumper Controller.
 * This program uses the Wireless Thumper communication protocol.  It controls
 * only the wheels using serial communication.  Designed for use with an XBee.
 */

// To connect an XBee use the three pins on the D0 header.
// Ground is on the outside, 5v power in the middle, Thumper Rx on inside.
// The Tx line for the Thumper is on D1 inside, but not needed for this program.

#include <WildThumperCom.h>

#define TEAM_NUMBER 4  // Change as appropriate for your team

// ==================== IO Pins =========================
#define PIN_LEFT_MOTOR_CH_A        3  // Left  motor H bridge, input A
#define PIN_LEFT_MOTOR_CH_B       11  // Left  motor H bridge, input B
#define PIN_RIGHT_MOTOR_CH_A       5  // Right motor H bridge, input A
#define PIN_RIGHT_MOTOR_CH_B       6  // Right motor H bridge, input B
#define PIN_BATTERY                0  // Analog input 00
#define PIN_RIGHT_MOTOR_CURRENT    6  // Analog input 06
#define PIN_LEFT_MOTOR_CURRENT     7  // Analog input 07
#define PIN_CHARGER               13  // Low=ON High=OFF
#define CHARGER_OFF                1  // Make sure charger is off (not useful)


// =============== H BRIDGE SETTINGS ====================
#define REVERSE 0  // Mode to go in reverse
#define BRAKE   1  // Mode to stop
#define FORWARD 2  // Mode to go forward
#define BATTERY_SHUTDOWN_MILLIVOLTS     6400     // This is the millivolt reading at which the battery is too low to continue. (6.4 volts)
#define MOTOR_SHUTDOWN_MILLIAMPS        8000     // overload current limit for motor (help reduce the risk of stalled motors drawing too much current) (8 amps)
#define COOLDOWN_TIME        100     // time in mS before motor is re-enabled after overload occurs


WildThumperCom wtc(TEAM_NUMBER);
unsigned long lastLeftMotorCurrentOverloadTime_ms = 0;
unsigned long lastRightMotorCurrentOverloadTime_ms = 0;
int leftMotorMode = BRAKE;                                           // 0=reverse, 1=brake, 2=forward
int rightMotorMode = BRAKE;                                          // 0=reverse, 1=brake, 2=forward
int leftMotorDutyCycle;                                              // PWM value for left  motor speed / brake
int rightMotorDutyCycle;                                             // PWM value for right motor speed / brake

void setup() {
  // Make sure the charger is always off (not useful and dangerous for our LiPo batteries)
  pinMode (PIN_CHARGER, OUTPUT);
  digitalWrite (PIN_CHARGER, CHARGER_OFF);

  Serial.begin(9600);  // Default speed used by our XBee boards
  wtc.registerWheelSpeedCallback(updateWheelSpeed);  // Call this function when a wheel speed command is received.
}

/** Update the wheel speed variables based on the new values received. */
void updateWheelSpeed(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
  leftMotorMode = leftMode;
  leftMotorDutyCycle = leftDutyCycle;
  rightMotorMode = rightMode;
  rightMotorDutyCycle = rightDutyCycle; 
}

void loop() {
  //--------------------- Check battery voltage and current draw of motors ---------------------
  int batteryVoltageAnalogReading = analogRead(PIN_BATTERY);      // read the battery voltage
  int leftCurrentAnalogReading = analogRead(PIN_LEFT_MOTOR_CURRENT);     // read left motor current draw
  int rightCurrentAnalogReading = analogRead(PIN_RIGHT_MOTOR_CURRENT);   // read right motor current draw
  int batteryInMillivolts = batteryVoltageAnalogReading * 14 + batteryVoltageAnalogReading / 2;      // 14.66 is the real value 14.5 is close enough
  int leftCurrentInMilliamps = leftCurrentAnalogReading * 20;     // left motor current draw in milliamps
  int rightCurrentInMilliamps = rightCurrentAnalogReading * 20;   // right motor current draw in milliamps
  

  if (batteryInMillivolts < BATTERY_SHUTDOWN_MILLIVOLTS) {      
      // Shut down the Wild Thumper if the battery voltage is too low (or spin in a circle or something).
      if ((millis() / 1000) % 2) {      
        updateWheelSpeed(FORWARD, FORWARD, 20, 20);
      } else {
        updateWheelSpeed(BRAKE, BRAKE, 0, 0);
      }
  }
  
  if (leftCurrentInMilliamps > MOTOR_SHUTDOWN_MILLIAMPS)                           // is motor current draw exceeding safe limit
  {
    analogWrite(PIN_LEFT_MOTOR_CH_A, 0);                            // turn off motors
    analogWrite(PIN_LEFT_MOTOR_CH_B, 0);                            // turn off motors
    lastLeftMotorCurrentOverloadTime_ms = millis();                 // record time of overload
  }

  if (rightCurrentInMilliamps > MOTOR_SHUTDOWN_MILLIAMPS)                          // is motor current draw exceeding safe limit
  {
    analogWrite(PIN_RIGHT_MOTOR_CH_A, 0);                            // turn off motors
    analogWrite(PIN_RIGHT_MOTOR_CH_B, 0);                            // turn off motors
    lastRightMotorCurrentOverloadTime_ms = millis();                   // record time of overload
  }    
  
  //--------------------- Set the motor speeds (if not in a stalled motor state) ---------------------
  if ((millis()-lastLeftMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)             
  {
    switch (leftMotorMode)
    {
      case FORWARD:                                             // left motor forward
        analogWrite(PIN_LEFT_MOTOR_CH_A, 0);
        analogWrite(PIN_LEFT_MOTOR_CH_B, leftMotorDutyCycle);
        break;
      case BRAKE:                                               // left motor brake
        analogWrite(PIN_LEFT_MOTOR_CH_A, leftMotorDutyCycle);
        analogWrite(PIN_LEFT_MOTOR_CH_B, leftMotorDutyCycle);
        break;
      case REVERSE:                                             // left motor reverse
        analogWrite(PIN_LEFT_MOTOR_CH_A, leftMotorDutyCycle);
        analogWrite(PIN_LEFT_MOTOR_CH_B, 0);
        break;
    }
  }
  if ((millis()-lastRightMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)
  {
    switch (rightMotorMode)
    {
      case FORWARD:                                             // right motor forward
        analogWrite(PIN_RIGHT_MOTOR_CH_A, 0);
        analogWrite(PIN_RIGHT_MOTOR_CH_B, rightMotorDutyCycle);
        break;
      case BRAKE:                                               // right motor brake
        analogWrite(PIN_RIGHT_MOTOR_CH_A, rightMotorDutyCycle);
        analogWrite(PIN_RIGHT_MOTOR_CH_B, rightMotorDutyCycle);
        break;
      case REVERSE:                                             // right motor reverse
        analogWrite(PIN_RIGHT_MOTOR_CH_A, rightMotorDutyCycle);
        analogWrite(PIN_RIGHT_MOTOR_CH_B, 0);
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
