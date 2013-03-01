#include "Arduino.h"
#include "ArmServosSpeedControlled.h"


ArmServosSpeedControlled::ArmServosSpeedControlled() {	
	_init();
}

ArmServosSpeedControlled::ArmServosSpeedControlled(byte joint1ServoPin, byte joint2ServoPin, byte joint3ServoPin, byte joint4ServoPin, byte joint5ServoPin, byte gripperServoPin) : ArmServos(joint1ServoPin, joint2ServoPin, joint3ServoPin, joint4ServoPin, joint5ServoPin, gripperServoPin) {
	_init();
}

void ArmServosSpeedControlled::_init() {
	ArmServos::_init();
	_millisecondsPerDegree[1] = DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_1;
	_millisecondsPerDegree[2] = DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_2;
	_millisecondsPerDegree[3] = DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_3;
	_millisecondsPerDegree[4] = DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_4;
	_millisecondsPerDegree[5] = DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_5;
	_targetJointAngles[1] = INITIAL_JOINT_1_ANGLE;
	_targetJointAngles[2] = INITIAL_JOINT_2_ANGLE;
	_targetJointAngles[3] = INITIAL_JOINT_3_ANGLE;
	_targetJointAngles[4] = INITIAL_JOINT_4_ANGLE;
	_targetJointAngles[5] = INITIAL_JOINT_5_ANGLE;
	_moveStartTimes[1] = 0;
	_moveStartTimes[2] = 0;
	_moveStartTimes[3] = 0;
	_moveStartTimes[4] = 0;
	_moveStartTimes[5] = 0;
}

void ArmServosSpeedControlled::updateServos() {
  // Calculate the position of each joint using ease in and out
  int absoluteTimeMs = millis();
  int currentTimeMs, totalMoveTimeMs, totalMoveDistance, startValue;
  
  for (int i = 1; i < NUM_SERVOS; i++) {
	  if (_targetJointAngles[i] != _servoAngles[i]) {	  
		  // Joint is not at the target location.
		  totalMoveDistance = _targetJointAngles[i] - _moveStartJointAngles[i];
		  totalMoveTimeMs = abs(totalMoveDistance) * _millisecondsPerDegree[i]; // Slower than 180 mspd beware.
		  currentTimeMs = absoluteTimeMs - _moveStartTimes[i];
		  if (currentTimeMs > totalMoveTimeMs) {
			  _servoAngles[i] = _targetJointAngles[i];
		  } else {
//			  Serial.print("Joint ");
//			  Serial.println(i);
//			  Serial.print("currentTimeMs = ");
//			  Serial.println(currentTimeMs);
//			  Serial.print("totalMoveTimeMs = ");
//			  Serial.println(totalMoveTimeMs);
//			  Serial.print("totalMoveDistance = ");
//			  Serial.println(totalMoveDistance);
//			  Serial.print("_moveStartJointAngles[i] = ");
//			  Serial.println(_moveStartJointAngles[i]);
			  _servoAngles[i] = _easeInOutAngle(currentTimeMs, totalMoveTimeMs, totalMoveDistance, _moveStartJointAngles[i]);
//			  Serial.print("Joint ");
//			  Serial.print(i);
//			  Serial.print(" to "); 
//			  Serial.println(_servoAngles[i]);
		  }
	  }
  }
  ArmServos::_updateServos();
}

int ArmServosSpeedControlled::_easeInOutAngle(int currentTimeMs, int totalMoveTimeMs, int totalMoveDistance, int startValue) {
  float t = (float) currentTimeMs;
  float d = (float) totalMoveTimeMs;
  float c = (float) totalMoveDistance;
  float b = (float) startValue;
  
  // Code from http://gizma.com/easing/
  // I added the cast back to an int the rest is used directly.
  t /= (d / 2.0);
  if (t < 1.0) return (int) (c / 2.0 * t * t * t + b);
  t -= 2.0;
  return (int) (c / 2.0 * (t * t * t + 2.0) + b);
}

void ArmServosSpeedControlled::setPosition(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) {
	int absoluteTimeMs = millis();
	_moveStartTimes[1] = absoluteTimeMs;
	_moveStartTimes[2] = absoluteTimeMs;
	_moveStartTimes[3] = absoluteTimeMs;
	_moveStartTimes[4] = absoluteTimeMs;
	_moveStartTimes[5] = absoluteTimeMs;
	_moveStartJointAngles[1] = _servoAngles[1];
	_moveStartJointAngles[2] = _servoAngles[2];
	_moveStartJointAngles[3] = _servoAngles[3];
	_moveStartJointAngles[4] = _servoAngles[4];
	_moveStartJointAngles[5] = _servoAngles[5];
	_targetJointAngles[1] = joint1Angle;
	_targetJointAngles[2] = joint2Angle;
	_targetJointAngles[3] = joint3Angle;
	_targetJointAngles[4] = joint4Angle;
	_targetJointAngles[5] = joint5Angle;
	updateServos();
}

void ArmServosSpeedControlled::setJointAngle(byte jointNumber, int angle) {
	_targetJointAngles[jointNumber] = angle;
	ArmServos::setJointAngle(jointNumber, angle);  // Note, individual settings don't use speed control.
}


void ArmServosSpeedControlled::setMillisecondsPerDegree(byte jointNumber, int millisecondsPerDegree) {
  _millisecondsPerDegree[jointNumber] = millisecondsPerDegree;
}

void ArmServosSpeedControlled::setAllMillisecondsPerDegree(int mspdJoint1, int mspdJoint2, int mspdJoint3, int mspdJoint4, int mspdJoint5) {
  _millisecondsPerDegree[1] = mspdJoint1;
  _millisecondsPerDegree[2] = mspdJoint2;
  _millisecondsPerDegree[3] = mspdJoint3;
  _millisecondsPerDegree[4] = mspdJoint4;
  _millisecondsPerDegree[5] = mspdJoint5;
}

int ArmServosSpeedControlled::getMillisecondsPerDegree(byte jointNumber) {
  return _millisecondsPerDegree[jointNumber];
}
