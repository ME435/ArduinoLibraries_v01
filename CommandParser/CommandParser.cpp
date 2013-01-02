#include "Arduino.h"
#include "CommandParser.h"

CommandParser::CommandParser() {
}

void CommandParser::init(Arm robotArm) {
	_robotArm = robotArm;

}

void CommandParser::parseCommand(String command) {
	Serial.print("Command = ");
	Serial.println(command);
	int spaceIndex = command.indexOf(' ');
	String angleStr;
	int angleInt, jointNumber = 1;
	if (command.startsWith("POSITION")) {
		angleStr = command;
		while (spaceIndex != -1 && jointNumber < 8) {
			angleStr = angleStr.substring(spaceIndex + 1);
			angleInt = angleStr.toInt();
			_robotArm.setJointAngle(jointNumber, angleInt);
			jointNumber++;
			spaceIndex = angleStr.indexOf(' ');
		}
	} else if (command.startsWith("GRIPPER")) {
		String gripperValueStr = command.substring(spaceIndex + 1);
		int gripperValueInt = gripperValueStr.toInt();
		_robotArm.setGripperDistance(gripperValueInt);
	} else if (command.startsWith("JOINT")) {
		String jointNumStr = command.substring(spaceIndex + 1, spaceIndex + 2);
		jointNumber = jointNumStr.toInt();
		// For now I'm ignoring the next keyword ANGLE
		int lastSpaceIndex = command.lastIndexOf(' ');
		angleStr = command.substring(lastSpaceIndex + 1);
		angleInt = angleStr.toInt();
		_robotArm.setJointAngle(jointNumber, angleInt);
	}
}
