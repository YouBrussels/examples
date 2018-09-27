#include "../src-gen/TimedStatemachineRequired.h"
#include "../src-gen/TimedStatemachine2Required.h"
#include "sc_pthread_timer_service.h"
#include "sc_cycle_runner.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_TIMER 5
#define MAX_RUNNER 2

TimedStatemachine statemachine;
TimedStatemachine2 statemachine2;
static pthread_mutex_t statechart_mutex = PTHREAD_MUTEX_INITIALIZER;


sc_pthread_timer_t timers[MAX_TIMER];
sc_cycle_runner_t cycle_runners[MAX_RUNNER];
sc_pthread_timer_service_t timer_service;
sc_cycle_runner_service_t cycle_runner_service;

extern sc_cycle_runner_methods_t sc_cycle_runner_methods;
extern sc_timer_service_methods_t sc_pthread_timer_service_methods;
sc_ts_connection_t ts_connection[] = { //
		{ //
						.ts_handle = &timer_service, //
						.ts_methods = &sc_pthread_timer_service_methods, //
						.sm_handle = &statemachine, //
						.sm_callback =
								(sc_time_event_callback_fp) timedStatemachine_raiseTimeEvent //
				},//
				{ //
						.ts_handle = &timer_service, //
						.ts_methods = &sc_pthread_timer_service_methods, //
						.sm_handle = &statemachine2, //
						.sm_callback =
								(sc_time_event_callback_fp) timedStatemachine2_raiseTimeEvent //
				}//
		}; //

sc_cr_connection_t sc_rc_connection[] = //
{//
		{ //
				.cr_handle = &cycle_runner_service, //
				.sm_handle = &statemachine, //
				.sm_runCycle = (sc_run_cycle_fp) timedStatemachine_runCycle //
		},//
		{ //
				.cr_handle = &cycle_runner_service, //
				.sm_handle = &statemachine2, //
				.sm_runCycle = (sc_run_cycle_fp) timedStatemachine2_runCycle //
		} //
}; //

int main() {
	puts("started");
	sc_pthread_timer_service_init(&timer_service, timers, MAX_TIMER,
			&statechart_mutex);
	sc_cycle_runner_init(&cycle_runner_service, cycle_runners, MAX_RUNNER, &statechart_mutex);
	timedStatemachine_init(&statemachine);
	timedStatemachine2_init(&statemachine2);
	puts("start enter");
	timedStatemachine_enter(&statemachine);
	timedStatemachine2_enter(&statemachine2);
	puts("enter done");
	sc_cycle_runner_methods.start(&sc_rc_connection[0], 100);
	sc_cycle_runner_methods.start(&sc_rc_connection[1], 100);
	for (;;);

	return 0;
}

extern void timedStatemachine_setTimer(TimedStatemachine* handle,
		const sc_eventid evid, const sc_integer time_ms,
		const sc_boolean periodic) {
	puts("some timer0 was set");
	sc_pthread_timer_service_methods.start(&ts_connection[0], evid, time_ms,
			periodic);
}

extern void timedStatemachine_unsetTimer(TimedStatemachine* handle,
		const sc_eventid evid) {
	puts("some timer0 was unset");
	sc_pthread_timer_service_methods.stop(&ts_connection[0], evid);
}

extern void timedStatemachine2_setTimer(TimedStatemachine2* handle,
		const sc_eventid evid, const sc_integer time_ms,
		const sc_boolean periodic) {
	puts("some timer1 was set");
	sc_pthread_timer_service_methods.start(&ts_connection[1], evid, time_ms,
			periodic);
}

extern void timedStatemachine2_unsetTimer(TimedStatemachine2* handle,
		const sc_eventid evid) {
	puts("some timer1 was unset");
	sc_pthread_timer_service_methods.stop(&ts_connection[1], evid);
}
