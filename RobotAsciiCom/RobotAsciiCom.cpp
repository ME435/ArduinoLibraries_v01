#include "Arduino.h"
#include "RobotAsciiCom.h"

RobotAsciiCom::RobotAsciiCom() {
	_nextOpenByteInMessageBuffer = 0;
	_positionCallback = NULL;
	_jointAngleCallback = NULL;
	_gripperCallback = NULL;
}

/**
 * Convenience method to handle multiple bytes.
 * Just calls the handleRxByte method with each byte individually.
 */
void RobotAsciiCom::handleRxBytes(byte newRxBytes[], int length) {
	for (int i = 0; i < length; i++) {
		handleRxByte(newRxBytes[i]);
	}
}

/**
 * Handle a newly received byte.
 * If the byte is the MESSAGE_TERMINATOR process the message.
 * If the byte is not the MESSAGE_TERMINATOR then just save it.
 */
void RobotAsciiCom::handleRxByte(byte newRxByte) {
	if (newRxByte == MESSAGE_TERMINATOR) {
		// Convert the rx message buffer to a String and parse.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = '\0';
		String rxStr = String(_rxMessageBuffer);
		_parseStringCommand(rxStr);
		_nextOpenByteInMessageBuffer = 0;
	} else {
		// Mid message.  Save the byte.
		_rxMessageBuffer[_nextOpenByteInMessageBuffer] = newRxByte;
		_nextOpenByteInMessageBuffer++;
	}
}

void RobotAsciiCom::registerPositionCallback(
		void (*positionCallback)(int joint1Angle, int joint2Angle,
				int joint3Angle, int joint4Angle, int joint5Angle)) {
	_positionCallback = positionCallback;
}

void RobotAsciiCom::registerJointAngleCallback(
		void (*jointAngleCallback)(byte joint, int jointAngle)) {
	_jointAngleCallback = jointAngleCallback;
}

void RobotAsciiCom::registerGripperCallback(
		void (*gripperCallback)(int gripperDistance)) {
	_gripperCallback = gripperCallback;
}

void RobotAsciiCom::_parseStringCommand(String command) {
//	Serial.print("Command = ");
//	Serial.println(command);
	int spaceIndex = command.indexOf(' ');
	String angleStr;
	if (command.startsWith("POSITION")) {
		int jointNumber = 1;
		int jointAngles[6]; // Note, gripper is joint 0, then normal order.
		angleStr = command;
		while (spaceIndex != -1 && jointNumber < 6) {
			angleStr = angleStr.substring(spaceIndex + 1);
			jointAngles[jointNumber] = angleStr.toInt();
			jointNumber++;
			spaceIndex = angleStr.indexOf(' ');
		}
		if (jointNumber == 6 && _positionCallback != NULL) {
			_positionCallback(jointAngles[1], jointAngles[2], jointAngles[3],
					jointAngles[4], jointAngles[5]);
		}
	} else if (command.startsWith("GRIPPER")) {
		String gripperValueStr = command.substring(spaceIndex + 1);
		if (_gripperCallback != NULL) {
			_gripperCallback(gripperValueStr.toInt());
		}
	} else if (command.startsWith("JOINT")) {
		String jointNumStr = command.substring(spaceIndex + 1, spaceIndex + 2);
		// For now I'm ignoring the next keyword ANGLE (lazy since it's the only command for a JOINT target)
		int lastSpaceIndex = command.lastIndexOf(' ');
		angleStr = command.substring(lastSpaceIndex + 1);
		if (_jointAngleCallback != NULL) {
			_jointAngleCallback(jointNumStr.toInt(), angleStr.toInt());
		}
	}
}
