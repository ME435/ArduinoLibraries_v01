#ifndef ArmServosSpeedControlled_h
#define ArmServosSpeedControlled_h

#include "Arduino.h"
#include "Servo.h"
#include "ArmServos.h"

#define DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_1 20
#define DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_2 40
#define DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_3 30
#define DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_4 20
#define DEFAULT_MILLISECONDS_PER_DEGREE_JOINT_5 10


class ArmServosSpeedControlled : public ArmServos
{
  public:
	ArmServosSpeedControlled();
	ArmServosSpeedControlled(byte joint1ServoPin, byte joint2ServoPin, byte joint3ServoPin, byte joint4ServoPin, byte joint5ServoPin, byte gripperServoPin);
    void setJointAngle(byte jointNumber, int angle);
    void setPosition(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
    void updateServos(void);	
	void setMillisecondsPerDegree(byte jointNumber, int millisecondsPerDegree);
	void setAllMillisecondsPerDegree(int mspdJoint1, int mspdJoint2, int mspdJoint3, int mspdJoint4, int mspdJoint5);
	int getMillisecondsPerDegree(byte jointNumber);		
  private:
    void _init(void);
	int _easeInOutAngle(int currentTimeMs, int totalMoveTimeMs, int totalMoveDistance, int startValue);
	int _targetJointAngles[NUM_SERVOS];
	int _moveStartJointAngles[NUM_SERVOS];
	unsigned long _moveStartTimes[NUM_SERVOS];
	int _millisecondsPerDegree[NUM_SERVOS];
};

#endif