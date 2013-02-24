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
#define COMMAND_WHEEL_SPEED 1

// Bytes within commands
#define WHEEL_SPEED_MESSAGE_LENGTH		6
#define WHEEL_SPEED_LEFT_MODE			2
#define WHEEL_SPEED_RIGHT_MODE			3
#define WHEEL_SPEED_LEFT_DUTY_CYCLE		4
#define WHEEL_SPEED_RIGHT_DUTY_CYCLE	5

class WildThumperCom
{
  public:
	WildThumperCom(byte teamNumber);
	void sendWheelSpeed(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
	void registerWheelSpeedCallback(void (* wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) );
	void handleRxByte(byte newRxByte);
  private:
	byte _teamNumber;
	byte _txMessageBuffer[MAX_MESSAGE_LENGTH];
	byte _rxMessageBuffer[MAX_MESSAGE_LENGTH];
	void _sendMessage(byte messageLength);
	void _sendByte(byte unescapedbyte);
	void (* _wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle);
	boolean _lastByteWasStartByte;
	boolean _lastByteWasEscapeByte;
	int _bytesRemainingInMessage;
	int _nextOpenByteInMessageBuffer;
	byte _crc;
	void _parseValidMessage();
};

#endif