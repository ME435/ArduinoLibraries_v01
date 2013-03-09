#ifndef TimerEventScheduler_h
#define TimerEventScheduler_h

/**
 * Adds timer events to an array and calls the callback function when it counts to zero.
 * 
 * TODO: Improve this discription.
 * 
 * Note, I don't know C++.  Ooops.
 * I only know C.  So I made a dynamic array of pointers using C technics (no C++ used).
 */

#include "Arduino.h"
#include "TimerEvent.h"

#define INITIAL_TIMER_EVENT_TABLE_CAPACITY 10

class TimerEventScheduler
{
  public:
	TimerEventScheduler();
	void addTimerEvent(TimerEvent *timerEventPtr);
  protected:
	void _doubleTimerEventTableCapacity(void);
	int _getNextTableIndex(void);
	// Low level Timer2 work from MsTimer2 playground code.
	void _setupTimer2(void);
	void _start(void);
	void _stop(void);
};

#endif


