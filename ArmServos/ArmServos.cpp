#include "Arduino.h"
#include "ArmServos.h"

ArmServos::ArmServos() {
	// CONSIDER: Allow a user to change the pin numbers
	_joint1Angle = 0;
	_joint2Angle = 90;
	_joint3Angle = 0;
	_joint4Angle = -90;
	_joint5Angle = 0;
	_gripperDistance = 50;
}

void ArmServos::attach() {
	_joint1Servo.attach(JOINT_1_SERVO_PIN);
	_joint2Servo.attach(JOINT_2_SERVO_PIN);
	_joint3Servo.attach(JOINT_3_SERVO_PIN);
	_joint4Servo.attach(JOINT_4_SERVO_PIN);
	_joint5Servo.attach(JOINT_5_SERVO_PIN);
	_gripperServo.attach(GRIPPER_SERVO_PIN);
	delay(100);  // May not be required, but saw in example code.
	_updateJointServos();
}

void ArmServos::_updateJointServos() {
	// Joint 1 DH goes from -90 to 90
	int mappedJoint1Angle = _joint1Angle + 90;
	// Joint 2 DH goes from 0 to 180
	int mappedJoint2Angle = _joint2Angle;
	// Joint 3 DH goes from 90 to -90
	int mappedJoint3Angle = 90 - _joint3Angle;
	// Joint 4 DH goes from -180 to 0
	int mappedJoint4Angle = _joint4Angle + 180;
	// Joint 5 DH goes from 0 to 180
	int mappedJoint5Angle = _joint5Angle;
	// 60 value on servo = 0 mm (less also 0)
	// 180 value on servo = 2.8 inches (71 mm)
	int mappedGripperDistance = _gripperDistance * 2 + 50;
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
void ArmServos::setPositionint joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
	_joint1Angle = joint1Angle;
	_joint2Angle = joint2Angle;
	_joint3Angle = joint3Angle;
	_joint4Angle = joint4Angle;
	_joint5Angle = joint5Angle;
	_updateJointServos();
}

// Consider: Change to an array to make code cleaner (low priority).
void ArmServos::setJointAngle(int jointNumber, int angle) {
	switch (jointNumber) {
	case 1:
		_joint1Angle = angle;
		break;
	case 2:
		_joint2Angle = angle;
		break;
	case 3:
		_joint3Angle = angle;
		break;
	case 4:
		_joint4Angle = angle;
		break;
	case 5:
		_joint5Angle = angle;
		break;
	}
	_updateJointServos();	
}

int ArmServos::getJointAngle(int jointNumber) {
	switch (jointNumber) {
	case 1:
		return _joint1Angle;
	case 2:
		return _joint2Angle;
	case 3:
		return _joint3Angle;
	case 4:
		return _joint4Angle;
	case 5:
		return _joint5Angle;
	}
}

void ArmServos::setGripperDistance(int distance) {
	_gripperDistance = distance;
	_updateJointServos();
}

int ArmServos::getGripperDistance() {
	return _gripperDistance;
}
