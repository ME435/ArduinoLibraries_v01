#ifndef ArmServos_h
#define ArmServos_h

#include "Arduino.h"
#include "Servo.h"

#define JOINT_1_SERVO_PIN 12
#define JOINT_2_SERVO_PIN 11
#define JOINT_3_SERVO_PIN 10
#define JOINT_4_SERVO_PIN 9
#define JOINT_5_SERVO_PIN 8
#define GRIPPER_SERVO_PIN 6

class ArmServos
{
  public:
	ArmServos();
    void attach();
    void setJointAngle(int jointNumber, int angle);
    int getJointAngle(int jointNumber);
    void setGripperDistance(int distance);
    int getGripperDistance();
    void setPositionint joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
  private:
    int _joint1Angle;
    int _joint2Angle;
    int _joint3Angle;
    int _joint4Angle;
    int _joint5Angle;
    int _gripperDistance;
    Servo _joint1Servo;
    Servo _joint2Servo;
    Servo _joint3Servo;
    Servo _joint4Servo;
    Servo _joint5Servo;
    Servo _gripperServo;
    void _updateJointServos();
};

#endif