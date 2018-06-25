// Insert some headers here

#include "ExtiaCounter.h"

// ====== Statics declarations ======

// millis() counter
volatile unsigned long ExtiaCounter::s_millis;
// Counters current values
volatile int ExtiaCounter::s_counterCurrentMillis[MAX_COUNTER];
// Period for callback execution
int ExtiaCounter::s_counterDuration[MAX_COUNTER];
// Callback on counter tick
void (*ExtiaCounter::s_counterCallback[MAX_COUNTER])(void);
// Counter state of execution (TRUE is ongoing while FALSE is stopped)
volatile bool ExtiaCounter::s_counterState[MAX_COUNTER];


// ====== End if statics declarations ======

/*
 * Initialize and start a counter. If the counter is already initialized
 * and/or started, this method will override its currents parameters and
 * restart it.
 *
 * @arg counter: the counter to initialize (must be between 0 and MAX_COUNTER - 1)
 * @arg duration: a duration (in milliseconds) for the counter
 * @arg callback: the callback to be called when counter reach duration
 *
 * @return: true if the counter is correctly initialized and started, false
 *          otherwise
 */
bool ExtiaCounter::setCounter(unsigned int counter, unsigned int duration, void (*callback)())
{
	if (counter > MAX_COUNTER)
		return false;

	s_counterDuration[counter] = duration;
	s_counterCurrentMillis[counter] = 0;
	s_counterCallback[counter] = callback;
	s_counterState[counter] = false;

	return true;
}

/*
 * Deactivate and reset a counter
 *
 * @arg counter: the counter to deactivate (must be between 0 and MAX_COUNTER - 1).
 *               @note if the counter is already deactivated, nothing will happen.
 *
 * @return: true if the counter passed in parameter is valid, false otherwise
 */
bool ExtiaCounter::resetCounter(unsigned int counter)
{
	if (counter > MAX_COUNTER)
		return false;

	ExtiaCounter::s_counterCurrentMillis[counter] = 0;
	ExtiaCounter::s_counterState[counter] = false;
}

/*
 * Constructor. Initialize stuff.
 */
ExtiaCounter::ExtiaCounter()
{
	// Deactivate interrupts
	noInterrupts();
	/*
	* Set in "Normal mode", meaning that the top timer value is 0xFF and
	* TOV flag is set when reaching this value.
	*/
	bitClear(TCCR1A, WGM10); // WGM10 = 0
	bitClear(TCCR1A, WGM11); // WGM11 = 0
	bitClear(TCCR1B, WGM12); // WGM12 = 0 (note that this flag is on TCCR1B unlike the two above)

	// No Clock prescale
	bitSet(TCCR1B, CS10); // CS10 = 1
	bitClear(TCCR1B, CS11); // CS11 = 0
	bitClear(TCCR1B, CS12); // CS12 = 0

	// Activate overflow interrupt
	bitSet(TIMSK1, TOIE1);

	// Reset overflow flag (just in case !)
	TIFR1 = 0b00000001;

	// Loading initial value in timer
	TCNT1 = TIMER_INITVAL;

	for (unsigned int i = 0; i < MAX_COUNTER; i++) {
		s_counterCurrentMillis[i] = 0;
		s_counterDuration[i] = 0;
		s_counterCallback[i] = 0;
		s_counterState[i] = false;
	}

	// (re)enable all interrupts
	interrupts();
}

/*
 * Returns the number of milliseconds since the library instantiation
 * been created.
 *
 * @return the number of milliseconds since the library instantiation
 */
unsigned long ExtiaCounter::millis()
{
	return s_millis;
}

/*
 * Pauses counter execution
 *
 * @return nothing
 */
void ExtiaCounter::pauseCounter(unsigned int counter)
{
    s_counterState[counter] = false;
}

/*
 * Starts counter execution
 *
 * @return nothing
 */
void ExtiaCounter::startCounter(unsigned int counter)
{
    s_counterState[counter]= true;
}

/*
 * Checks whether the counter is running or not
 *
 * @return the current state of the counter
 */
bool ExtiaCounter::isRunning(unsigned int counter)
{
    return s_counterState[counter];
}

// Interruption routine
ISR(TIMER1_OVF_vect)
{
	// Loading initial value in timer
	TCNT1 = TIMER_INITVAL;

	ExtiaCounter::s_millis++;

	for (unsigned int i = 0; i < MAX_COUNTER; i++) {
        if (ExtiaCounter::s_counterState[i]){
            if (ExtiaCounter::s_counterCallback[i] != 0) { // Is a callback attached ? If not, do not bother and do nothing.
                ExtiaCounter::s_counterCurrentMillis[i]++;
                if (ExtiaCounter::s_counterCurrentMillis[i] > ExtiaCounter::s_counterDuration[i]) {
                    ExtiaCounter::s_counterCurrentMillis[i] = 0;
                    ExtiaCounter::s_counterState[i] = false;
                    (*ExtiaCounter::s_counterCallback[i])();	   // execute callback
                }
            }
        }

	}
}
