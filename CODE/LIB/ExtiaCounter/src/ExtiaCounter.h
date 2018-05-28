// Insert some headers here

#ifndef EXTIA_COUNTER_H_
#define EXTIA_COUNTER_H_

#include "Arduino.h"

extern "C" void TIMER1_OVF_vect(void) __attribute__((signal));

#define MAX_COUNTER 3

// 65536 - 16000 => TIMER1 max value (16bits) - 16Mhz master clock, no prescaler
#define TIMER_INITVAL 49536

class ExtiaCounter {
public:
	ExtiaCounter();

	bool setCounter(unsigned int counter, unsigned int duration, void (*callback)());
	bool resetCounter(unsigned int counter);
	void startCounter(unsigned int counter);
	void pauseCounter(unsigned int counter);
	unsigned long millis();

	friend void ::TIMER1_OVF_vect(void);

private:

	// millis() value
	static volatile unsigned long s_millis;

	// Counters current values
	static volatile int s_counterCurrentMillis[MAX_COUNTER];

	// Period for callback execution
	static int s_counterDuration[MAX_COUNTER];

	// Callback on counter tick
	static void (*s_counterCallback[MAX_COUNTER])(void);

	// Boolean used to pause execution
	static bool s_counterState[MAX_COUNTER];
};

#endif // !EXTIA_COUNTER_H_
