#ifndef RobotAsciiCom_h
#define RobotAsciiCom_h

#include "Arduino.h"

#define MAX_ASCII_MESSAGE_LENGTH 	64
#define MESSAGE_TERMINATOR 			10

class RobotAsciiCom
{
  public:
	RobotAsciiCom();
    void handleRxByte(byte newRxByte);
    void handleRxBytes(byte newRxBytes[], int length);
    void registerPositionCallback(void (* positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) );
    void registerJointAngleCallback(void (* jointAngleCallback)(byte joint, int jointAngle) );
    void registerGripperCallback(void (* gripperCallback)(int gripperDistance) );    	
  private:
	byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
	int _nextOpenByteInMessageBuffer;
    void (* _positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
    void (* _jointAngleCallback)(byte joint, int jointAngle);
    void (* _gripperCallback)(int gripperDistance);
    void _parseStringCommand(String command);
};

#endif
