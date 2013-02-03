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

#include <WirelessThumperCom.h>

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
#define BATTERY_SHUTDOWN_VOLTAGE       410     // This is the voltage at which the battery is too low to continue.
#define MAX_LEFT_AMPS        800     // overload current limit for left motor (help reduce the risk of stalled motors drawing too much current)
#define MAX_RIGHT_AMPS       800     // overload current limit for right motor (help reduce the risk of stalled motors drawing too much current)
#define COOLDOWN_TIME        100     // time in mS before motor is re-enabled after overload occurs

WirelessThumperCom wtc(TEAM_NUMBER);
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
  int batteryVoltageReading = analogRead(PIN_BATTERY);             // read the battery voltage
  int leftCurrentReading = analogRead(PIN_LEFT_MOTOR_CURRENT);     // read left motor current draw
  int rightCurrentReading = analogRead(PIN_RIGHT_MOTOR_CURRENT);   // read right motor current draw
  
  if (batteryVoltageReading < BATTERY_SHUTDOWN_VOLTAGE) {
      // TODO: Shut down the Wild Thumper if the battery voltage is too low (or spin in a circle or something).
  }
  
  if (leftCurrentReading > MAX_LEFT_AMPS)                           // is motor current draw exceeding safe limit
  {
    analogWrite(PIN_LEFT_MOTOR_CH_A, 0);                            // turn off motors
    analogWrite(PIN_LEFT_MOTOR_CH_B, 0);                            // turn off motors
    lastLeftMotorCurrentOverloadTime_ms = millis();                 // record time of overload
  }

  if (rightCurrentReading > MAX_RIGHT_AMPS)                          // is motor current draw exceeding safe limit
  {
    analogWrite(PIN_RIGHT_MOTOR_CH_A, 0);                            // turn off motors
    analogWrite(PIN_RIGHT_MOTOR_CH_B, 0);                            // turn off motors
    lastRightMotorCurrentOverloadTime_ms=millis();                   // record time of overload
  }    
  
  //--------------------- Set the motor speeds (if not in a stalled motor state) ---------------------
  if ((millis()-lastLeftMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)             
  {
    switch (leftMotorMode)
    {
      case FORWARD:                                             // left motor forward
        analogWrite(PIN_LEFT_MOTOR_CH_A,0);
        analogWrite(PIN_LEFT_MOTOR_CH_B,leftMotorDutyCycle);
        break;
      case BRAKE:                                               // left motor brake
        analogWrite(PIN_LEFT_MOTOR_CH_A,leftMotorDutyCycle);
        analogWrite(PIN_LEFT_MOTOR_CH_B,leftMotorDutyCycle);
        break;
      case REVERSE:                                             // left motor reverse
        analogWrite(PIN_LEFT_MOTOR_CH_A,leftMotorDutyCycle);
        analogWrite(PIN_LEFT_MOTOR_CH_B,0);
        break;
    }
  }
  if ((millis()-lastRightMotorCurrentOverloadTime_ms) > COOLDOWN_TIME)
  {
    switch (rightMotorMode)
    {
      case FORWARD:                                             // right motor forward
        analogWrite(PIN_RIGHT_MOTOR_CH_A,0);
        analogWrite(PIN_RIGHT_MOTOR_CH_B,rightMotorDutyCycle);
        break;
      case BRAKE:                                               // right motor brake
        analogWrite(PIN_RIGHT_MOTOR_CH_A,rightMotorDutyCycle);
        analogWrite(PIN_RIGHT_MOTOR_CH_B,rightMotorDutyCycle);
        break;
      case REVERSE:                                             // right motor reverse
        analogWrite(PIN_RIGHT_MOTOR_CH_A,rightMotorDutyCycle);
        analogWrite(PIN_RIGHT_MOTOR_CH_B,0);
        break;
    }
  }
}

/** Send all bytes received to the Wireless Thumper Com object. */
void serialEvent() {
  while (Serial.available()) {
    wtc.parseByte(Serial.read());
  }
}
