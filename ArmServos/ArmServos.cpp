#include "Arduino.h"
#include "ArmServos.h"

ArmServos::ArmServos() {
	_joint1ServoPin = DEFAULT_JOINT_1_SERVO_PIN;
	_joint2ServoPin = DEFAULT_JOINT_2_SERVO_PIN;
	_joint3ServoPin = DEFAULT_JOINT_3_SERVO_PIN;
	_joint4ServoPin = DEFAULT_JOINT_4_SERVO_PIN;
	_joint5ServoPin = DEFAULT_JOINT_5_SERVO_PIN;
	_gripperServoPin = DEFAULT_GRIPPER_SERVO_PIN;
	_init();
}

ArmServos::ArmServos(byte joint1ServoPin, byte joint2ServoPin,
		byte joint3ServoPin, byte joint4ServoPin, byte joint5ServoPin,
		byte gripperServoPin) {
	_joint1ServoPin = joint1ServoPin;
	_joint2ServoPin = joint2ServoPin;
	_joint3ServoPin = joint3ServoPin;
	_joint4ServoPin = joint4ServoPin;
	_joint5ServoPin = joint5ServoPin;
	_gripperServoPin = gripperServoPin;
	_init();
}

void ArmServos::_init() {
	_servoAngles[1] = INITIAL_JOINT_1_ANGLE;
	_servoAngles[2] = INITIAL_JOINT_2_ANGLE;
	_servoAngles[3] = INITIAL_JOINT_3_ANGLE;
	_servoAngles[4] = INITIAL_JOINT_4_ANGLE;
	_servoAngles[5] = INITIAL_JOINT_5_ANGLE;
	_servoAngles[GRIPPER_SERVO_INDEX] = INITIAL_GRIPPER_DISTANCE;	
}

void ArmServos::attach() {
	_updateServos();
	_joint1Servo.attach(_joint1ServoPin);
	_joint2Servo.attach(_joint2ServoPin);
	_joint3Servo.attach(_joint3ServoPin);
	_joint4Servo.attach(_joint4ServoPin);
	_joint5Servo.attach(_joint5ServoPin);
	_gripperServo.attach(_gripperServoPin);
	delay(100);  // May not be required, but saw in example code.
}

void ArmServos::_updateServos() {
	// Joint 1 DH goes from -90 to 90
	int mappedJoint1Angle = _servoAngles[1] + 90;
	// Joint 2 DH goes from 0 to 180
	int mappedJoint2Angle = _servoAngles[2];
	// Joint 3 DH goes from 90 to -90
	int mappedJoint3Angle = 90 - _servoAngles[3];
	// Joint 4 DH goes from -180 to 0
	int mappedJoint4Angle = _servoAngles[4] + 180;
	// Joint 5 DH goes from 0 to 180
	int mappedJoint5Angle = _servoAngles[5];
	// 60 value on servo = 0 mm (less also 0)
	// 180 value on servo = 2.8 inches (71 mm)
	int mappedGripperDistance = _servoAngles[GRIPPER_SERVO_INDEX] * 2 + 50;
	// Constrain to 0 to 180 values.
	mappedJoint1Angle = constrain(mappedJoint1Angle, 0, 180);
	mappedJoint2Angle = constrain(mappedJoint2Angle, 0, 180);
	mappedJoint3Angle = constrain(mappedJoint3Angle, 0, 180);
	mappedJoint4Angle = constrain(mappedJoint4Angle, 0, 180);
	mappedJoint5Angle = constrain(mappedJoint5Angle, 0, 180);
	mappedGripperDistance = constrain(mappedGripperDistance, 0, 180);
	// Update the output pins.
	_joint1Servo.write(mappedJoint1Angle);
	_joint2Servo.write(mappedJoint2Angle);
	_joint3Servo.write(mappedJoint3Angle);
	_joint4Servo.write(mappedJoint4Angle);
	_joint5Servo.write(mappedJoint5Angle);
	_gripperServo.write(mappedGripperDistance);

//	Serial.print("Moving arm to ");
//	Serial.print(mappedJoint1Angle);
//	Serial.print(" ");
//	Serial.print(mappedJoint2Angle);
//	Serial.print(" ");
//	Serial.print(mappedJoint3Angle);
//	Serial.print(" ");
//	Serial.print(mappedJoint4Angle);
//	Serial.print(" ");
//	Serial.print(mappedJoint5Angle);
//	Serial.print(" gripper to ");
//	Serial.print(mappedGripperDistance);
//	Serial.println(".");
}

// For the common case of setting 5 joints at once.
void ArmServos::setPosition(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
	_servoAngles[1] = joint1Angle;
	_servoAngles[2] = joint2Angle;
	_servoAngles[3] = joint3Angle;
	_servoAngles[4] = joint4Angle;
	_servoAngles[5] = joint5Angle;
	_updateServos();
}

void ArmServos::setJointAngle(byte jointNumber, int angle) {
	if (jointNumber > 0 && jointNumber < NUM_SERVOS) {
		_servoAngles[jointNumber] = angle;		
	}
	_updateServos();
}

int ArmServos::getJointAngle(byte jointNumber) {
	return _servoAngles[jointNumber];
}

void ArmServos::setGripperDistance(int distance) {
	_servoAngles[GRIPPER_SERVO_INDEX] = distance;
	_updateServos();
}

int ArmServos::getGripperDistance() {
	return _servoAngles[GRIPPER_SERVO_INDEX];
}
