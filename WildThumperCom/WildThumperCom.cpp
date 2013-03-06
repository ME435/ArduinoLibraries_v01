#include "Arduino.h"
#include "WildThumperCom.h"


WildThumperCom::WildThumperCom(byte teamNumber) {
	_teamNumber = teamNumber;
	_wheelSpeedCallback = NULL;
	_lastByteWasStartByte = false;
	_lastByteWasEscapeByte = false;
	_bytesRemainingInMessage = -1;

}


void WildThumperCom::sendWheelSpeed(byte leftMode, byte rightMode,
		byte leftDutyCycle, byte rightDutyCycle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_WHEEL_SPEED;
	_txMessageBuffer[WHEEL_SPEED_LEFT_MODE] = leftMode;
	_txMessageBuffer[WHEEL_SPEED_RIGHT_MODE] = rightMode;
	_txMessageBuffer[WHEEL_SPEED_LEFT_DUTY_CYCLE] = leftDutyCycle;
	_txMessageBuffer[WHEEL_SPEED_RIGHT_DUTY_CYCLE] = rightDutyCycle;
	_sendMessage (WHEEL_SPEED_MESSAGE_LENGTH);
}


void WildThumperCom::sendPosition(int joint1Angle, int joint2Angle,
		int joint3Angle, int joint4Angle, int joint5Angle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_ARM_POSITION;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_1_LSB] = (byte) joint1Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_1_MSB] = (byte)(joint1Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_2_LSB] = (byte) joint2Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_2_MSB] = (byte)(joint2Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_3_LSB] = (byte) joint3Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_3_MSB] = (byte)(joint3Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_4_LSB] = (byte) joint4Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_4_MSB] = (byte)(joint4Angle >> 8);
	_txMessageBuffer[SET_ARM_POSITION_JOINT_5_LSB] = (byte) joint5Angle;
	_txMessageBuffer[SET_ARM_POSITION_JOINT_5_MSB] = (byte)(joint5Angle >> 8);
	_sendMessage (SET_ARM_POSITION_LENGTH);
}


void WildThumperCom::sendJointAngle(byte jointNumber, int jointAngle) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_JOINT_ANGLE;
	_txMessageBuffer[SET_JOINT_ANGLE_JOINT_NUMBER] = jointNumber;
	_txMessageBuffer[SET_JOINT_ANGLE_ANGLE_LSB] = (byte) jointAngle;
	_txMessageBuffer[SET_JOINT_ANGLE_ANGLE_MSB] = (byte)(jointAngle >> 8);
	_sendMessage (SET_JOINT_ANGLE_LENGTH);
}


void WildThumperCom::sendGripperDistance(int gripperDistance) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_SET_GRIPPER_DISTANCE;
	_txMessageBuffer[SET_GRIPPER_DISTANCE_LSB] = (byte) gripperDistance;
	_txMessageBuffer[SET_GRIPPER_DISTANCE_MSB] = (byte)(gripperDistance >> 8);
	_sendMessage (SET_GRIPPER_DISTANCE_LENGTH);
}


void WildThumperCom::sendBatteryVoltageRequest() {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REQUEST;
	_sendMessage (BATTERY_VOLTAGE_REQUEST_LENGTH);
}


void WildThumperCom::sendBatteryVoltageReply(int batteryMillivolts) {
	_txMessageBuffer[TEAM_NUMBER_BYTE] = _teamNumber;
	_txMessageBuffer[COMMAND_BYTE] = COMMAND_BATTERY_VOLTAGE_REPLY;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_LSB] = (byte) batteryMillivolts;
	_txMessageBuffer[BATTERY_VOLTAGE_REPLY_MSB] = (byte)(
			batteryMillivolts >> 8);
	_sendMessage (BATTERY_VOLTAGE_REPLY_LENGTH);
}


void WildThumperCom::_sendMessage(byte messageLength) {
	byte crc = 0;
	Serial.write(START_BYTE);
	_sendByte(messageLength);
	for (int i = 0; i < messageLength; i++) {
		_sendByte (_txMessageBuffer[i]);
		crc += _txMessageBuffer[i];
	}
	_sendByte(-crc);
}


void WildThumperCom::_sendByte(byte unescapedbyte) {
	if (unescapedbyte == START_BYTE || unescapedbyte == ESCAPE_BYTE) {
		Serial.write(ESCAPE_BYTE);
		Serial.write(unescapedbyte ^ ESCAPE_XOR);
	} else {
		Serial.write(unescapedbyte);
	}
}


void WildThumperCom::registerWheelSpeedCallback(
		void (*wheelSpeedCallback)(byte leftMode, byte rightMode,
				byte leftDutyCycle, byte rightDutyCycle)) {
	_wheelSpeedCallback = wheelSpeedCallback;
}


void WildThumperCom::registerPositionCallback(
		void (*positionCallback)(int joint1Angle, int joint2Angle,
				int joint3Angle, int joint4Angle, int joint5Angle)) {
	_positionCallback = positionCallback;
}


void WildThumperCom::registerJointAngleCallback(
		void (*jointAngleCallback)(byte jointNumber, int jointAngle)) {
	_jointAngleCallback = jointAngleCallback;
}


void WildThumperCom::registerGripperCallback(
		void (*gripperCallback)(int gripperDistance)) {
	_gripperCallback = gripperCallback;
}


void WildThumperCom::registerBatteryVoltageRequestCallback(
		void (*batteryVoltageRequestCallback)(void)) {
	_batteryVoltageRequestCallback = batteryVoltageRequestCallback;
}


void WildThumperCom::registerBatteryVoltageReplyCallback(
		void (*batteryVoltageRequestCallback)(int batteryMillivolts)) {
	_batteryVoltageReplyCallback = batteryVoltageReplyCallback;
}


void WildThumperCom::handleRxByte(byte newRxByte) {
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
	switch (_rxMessageBuffer[COMMAND_BYTE]) {
	case COMMAND_WHEEL_SPEED:
		if (_wheelSpeedCallback != NULL) {
			_wheelSpeedCallback(_rxMessageBuffer[WHEEL_SPEED_LEFT_MODE],
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
