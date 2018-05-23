/*
 * PedalBox.h
 *
 *  Created on: Jan 12, 2017
 *      Author: ben
 */

#ifndef PEDALBOX_H_
#define PEDALBOX_H_

#include "stm32f4xx_hal.h"

typedef enum {
	PEDALBOX_STATUS_ERROR = 1, //generic error
	PEDALBOX_STATUS_ERROR_EOR, // encoder out of range
	PEDALBOX_STATUS_ERROR_APPSIMP,  //APPS Implausibility error, EV 2.3.5,
	PEDALBOX_STATUS_ERROR_APPSIMP_PREV,  //APPS Implausibility error, provisional (before it has lasted .1 second)
	PEDALBOX_STATUS_ERROR_BPIMP,	//brake pedal implaus //EV 2.5.1,
	PEDALBOX_STATUS_NO_ERROR = 0
} Pedalbox_status_t;

// Structure to hold data passed through the queue to pedalBoxMsgHandler
typedef struct _pedalbox_msg {
	//Pedalbox_status_t 		EOR; 				// EV 2.4.6: Encoder out of range
	//Pedalbox_status_t 		APPS_Implausible; 	// EV 2.3.5
	int 				throttle1_raw;		// raw throttle data from pedalbox
	int 				throttle2_raw;
	int  				brake1_raw;
	int 				brake2_raw;

} Pedalbox_msg_t;

typedef struct {
	uint16_t				count;
	uint16_t				period_ms;
	Pedalbox_msg_t			msg;
} GeneratePedalboxMessages_t;

#endif /* PEDALBOX_H_ */
