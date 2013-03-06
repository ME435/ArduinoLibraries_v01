#ifndef WildThumperCom_h
#define WildThumperCom_h

#include "Arduino.h"

// Special protocol bytes
#define START_BYTE 			0x7E
#define ESCAPE_BYTE 		0x7D
#define ESCAPE_XOR 			0x20

// Message details
#define MAX_MESSAGE_LENGTH 	32
#define TEAM_NUMBER_BYTE  	0
#define COMMAND_BYTE 		1

// Commands
#define COMMAND_WHEEL_SPEED              1
#define COMMAND_SET_ARM_POSITION         2
#define COMMAND_SET_JOINT_ANGLE          3
#define COMMAND_SET_GRIPPER_DISTANCE     4
#define COMMAND_BATTERY_VOLTAGE_REQUEST  5
#define COMMAND_BATTERY_VOLTAGE_REPLY    6

// Bytes within commands
#define WHEEL_SPEED_MESSAGE_LENGTH		6
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1
#define WHEEL_SPEED_LEFT_MODE			2
#define WHEEL_SPEED_RIGHT_MODE			3
#define WHEEL_SPEED_LEFT_DUTY_CYCLE		4
#define WHEEL_SPEED_RIGHT_DUTY_CYCLE	5

#define SET_ARM_POSITION_LENGTH 12
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1
#define SET_ARM_POSITION_JOINT_1_LSB 2
#define SET_ARM_POSITION_JOINT_1_MSB 3
#define SET_ARM_POSITION_JOINT_2_LSB 4
#define SET_ARM_POSITION_JOINT_2_MSB 5
#define SET_ARM_POSITION_JOINT_3_LSB 6
#define SET_ARM_POSITION_JOINT_3_MSB 7
#define SET_ARM_POSITION_JOINT_4_LSB 8
#define SET_ARM_POSITION_JOINT_4_MSB 9
#define SET_ARM_POSITION_JOINT_5_LSB 10
#define SET_ARM_POSITION_JOINT_5_MSB 11

#define SET_JOINT_ANGLE_LENGTH 5
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1
#define SET_JOINT_ANGLE_JOINT_NUMBER 2
#define SET_JOINT_ANGLE_ANGLE_LSB    3
#define SET_JOINT_ANGLE_ANGLE_MSB    4

#define SET_GRIPPER_DISTANCE_LENGTH 4
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1
#define SET_GRIPPER_DISTANCE_LSB 2
#define SET_GRIPPER_DISTANCE_MSB 3


#define BATTERY_VOLTAGE_REQUEST_LENGTH 2
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1

#define BATTERY_VOLTAGE_REPLY_LENGTH 4
// TEAM_NUMBER_BYTE is 0, COMMAND_BYTE is 1
#define BATTERY_VOLTAGE_REPLY_LSB 2
#define BATTERY_VOLTAGE_REPLY_MSB 3


class WildThumperCom
{
  public:
	WildThumperCom(byte teamNumber);
	void sendWheelSpeed(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
	void sendPosition(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
	void sendJointAngle(byte jointNumber, int jointAngle);
	void sendGripperDistance(int gripperDistance);
	void sendBatteryVoltageRequest();
	void sendBatteryVoltageReply(int batteryMillivolts);	
	void registerWheelSpeedCallback(void (* wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) );
    void registerPositionCallback(void (* positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle) );
    void registerJointAngleCallback(void (* jointAngleCallback)(byte jointNumber, int jointAngle) );
    void registerGripperCallback(void (* gripperCallback)(int gripperDistance) );
    void registerBatteryVoltageRequestCallback(void (* batteryVoltageRequestCallback)(void) );
    void registerBatteryVoltageReplyCallback(void (* batteryVoltageReplyCallback)(int batteryMillivolts) );	
	void handleRxByte(byte newRxByte);
  private:
	byte _teamNumber;
	byte _txMessageBuffer[MAX_MESSAGE_LENGTH];
	byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
	void _sendMessage(byte messageLength);
	void _sendByte(byte unescapedbyte);
	void (* _wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
    void (* _positionCallback)(int joint1Angle, int joint2Angle, int joint3Angle, int joint4Angle, int joint5Angle);
    void (* _jointAngleCallback)(byte joint, int jointAngle);
    void (* _gripperCallback)(int gripperDistance);
    void (* _batteryVoltageRequestCallback)(void);
    void (* _batteryVoltageReplyCallback)(int batteryMillivolts);
	boolean _lastByteWasStartByte;
	boolean _lastByteWasEscapeByte;
	int _bytesRemainingInMessage;
	int _nextOpenByteInMessageBuffer;
	byte _crc;
	void _parseValidMessage();
};

#endif