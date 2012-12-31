#ifndef CommandParser_h
#define CommandParser_h

#include "Arduino.h"
#include "Arm.h"

class CommandParser
{
  public:
	CommandParser();
	void init(Arm robotArm);
    void parseCommand(String command);
  private:
    Arm _robotArm;
};

#endif