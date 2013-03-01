#include "Arduino.h"
#include "WildThumperCom.h"

WildThumperCom::WildThumperCom(byte teamNumber) {
	_teamNumber = teamNumber;
	_wheelSpeedCallback = NULL;
	_lastByteWasStartByte = false;
	_lastByteWasEscapeByte = false;
	_bytesRemainingInMessage = -1;
	
}

void WildThumperCom::sendWheelSpeed(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) {
	_txMessageBuffer[0] = _teamNumber;
	_txMessageBuffer[1] = COMMAND_WHEEL_SPEED;
	_txMessageBuffer[2] = leftMode;
	_txMessageBuffer[3] = rightMode;
	_txMessageBuffer[4] = leftDutyCycle;
	_txMessageBuffer[5] = rightDutyCycle;
	_sendMessage(WHEEL_SPEED_MESSAGE_LENGTH);
}

void WildThumperCom::_sendMessage(byte messageLength) {
	byte crc = 0;
	Serial.write(START_BYTE);
	_sendByte(messageLength);
	for (int i = 0; i < messageLength; i++) {
		_sendByte(_txMessageBuffer[i]);
		crc += _txMessageBuffer[i];
	}
	_sendByte(-crc);
}

void WildThumperCom::_sendByte(byte unescapedbyte) {
	if (unescapedbyte == START_BYTE ||
		unescapedbyte == ESCAPE_BYTE) {
		Serial.write(ESCAPE_BYTE);
		Serial.write(unescapedbyte ^ ESCAPE_XOR);		
	} else {
		Serial.write(unescapedbyte);
	}
}

void WildThumperCom::registerWheelSpeedCallback(void (* wheelSpeedCallback)(byte leftMode, byte rightMode, byte leftDutyCycle, byte rightDutyCycle) ) {
	_wheelSpeedCallback = wheelSpeedCallback;
}

void WildThumperCom::parseByte(byte newRxByte) {
	// Highest priority is the start byte.
	if (newRxByte == START_BYTE) {
		_lastByteWasStartByte = true;
		_lastByteWasEscapeByte = false;
		return;
	} else if (!_lastByteWasStartByte) {
		// Make sure we are expecting bytes (ignore unexpected bytes).
		if (_bytesRemainingInMessage < 0) {
			return;
		}
	}
	// Next handle the escaping mechanism.
	if (_lastByteWasEscapeByte) {
		newRxByte ^= ESCAPE_XOR;
	} else {
		if (newRxByte == ESCAPE_BYTE) {
			_lastByteWasEscapeByte = true;
			return;
		}
	}
	_lastByteWasEscapeByte = false;
	// Next handle the length byte
	if (_lastByteWasStartByte) {
		// This is the length byte.
		_bytesRemainingInMessage = newRxByte;
		_crc = 0;
		_nextOpenByteInMessageBuffer = 0;
		_lastByteWasStartByte = false;
		return;
	}
	// Handle this data byte.
	_crc += newRxByte;
	if (_bytesRemainingInMessage > 0) {
		// Receiving bytes in the message.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	} else {
		// Message just finished time to validate the CRC byte.
		if (_crc == 0) {
			_parseValidMessage();
		}
	}
	_bytesRemainingInMessage--;
}

void WildThumperCom::_parseValidMessage() {
	byte teamNumber = _rxMessageBuffer[TEAM_NUMBER_BYTE];
	if (teamNumber != _teamNumber) {
		// Silently do nothing if it's not our team.
		return;
	}
	switch(_rxMessageBuffer[COMMAND_BYTE]) {
	case COMMAND_WHEEL_SPEED:
		if (_wheelSpeedCallback != NULL) {
			_wheelSpeedCallback(
					_rxMessageBuffer[WHEEL_SPEED_LEFT_MODE],
					_rxMessageBuffer[WHEEL_SPEED_RIGHT_MODE],
					_rxMessageBuffer[WHEEL_SPEED_LEFT_DUTY_CYCLE],
					_rxMessageBuffer[WHEEL_SPEED_RIGHT_DUTY_CYCLE]);
		}
		break;
	default:
		// Silently do nothing with unknown commands
		break;
	}
}
