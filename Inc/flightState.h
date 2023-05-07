#ifndef __FLIGHTSTATE_H
#define __FLIGHTSTATE_H

typedef enum FlightState {
	STARTUP = 0,
	WAIT_FOR_FIX = 1,
	OPERATION = 5
};

#endif
