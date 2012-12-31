#include "Arduino.h"
#include "Arm.h"

Arm::Arm() {
	// TODO: Allow a user to change the pin numbers
	_joint1Angle = 0;
	_joint2Angle = 90;
	_joint3Angle = 0;
	_joint4Angle = -90;
	_joint5Angle = 0;
	_gripperDistance = 50;

	_joint1Servo.attach(JOINT_1_SERVO_PIN);
	_joint2Servo.attach(JOINT_2_SERVO_PIN);
	_joint3Servo.attach(JOINT_3_SERVO_PIN);
	_joint4Servo.attach(JOINT_4_SERVO_PIN);
	_joint5Servo.attach(JOINT_5_SERVO_PIN);
	_gripperServo.attach(GRIPPER_SERVO_PIN);
}

void Arm::init() {
	_updateJointServos();
}

void Arm::_updateJointServos() {
	int mappedJoint1Angle = _joint1Angle;
	int mappedJoint2Angle = _joint2Angle;
	int mappedJoint3Angle = _joint3Angle;
	int mappedJoint4Angle = _joint4Angle;
	int mappedJoint5Angle = _joint5Angle;
	int mappedGripperDistance = _gripperDistance;
	_joint1Servo.write(mappedJoint1Angle);
	_joint2Servo.write(mappedJoint2Angle);
	_joint3Servo.write(mappedJoint3Angle);
	_joint4Servo.write(mappedJoint4Angle);
	_joint5Servo.write(mappedJoint5Angle);
	_gripperServo.write(mappedGripperDistance);
	Serial.print("Moving arm to ");
	Serial.print(mappedJoint1Angle);
	Serial.print(" ");
	Serial.print(mappedJoint2Angle);
	Serial.print(" ");
	Serial.print(mappedJoint3Angle);
	Serial.print(" ");
	Serial.print(mappedJoint4Angle);
	Serial.print(" ");
	Serial.print(mappedJoint5Angle);
	Serial.print(" gripper to ");
	Serial.print(mappedGripperDistance);
	Serial.println(".");
}

void Arm::setJointAngle(int jointNumber, int angle) {
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

int Arm::getJointAngle(int jointNumber) {
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

void Arm::setGripperDistance(int distance) {
	_gripperDistance = distance;
	_updateJointServos();
}

int Arm::getGripperDistance() {
	return _gripperDistance;
}
