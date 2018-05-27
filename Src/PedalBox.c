/*
 * PedalBox.c
 *
 *  Created on: Jan 12, 2017
 *      Author: ben
 */

#include "PedalBox.h"
#include "car.h"


void taskPedalBoxMsgHandler() {
/***************************************************************************
*
*     Function Information
*
*     Name of Function: pedalBoxMessageHandler
*
*     Programmer's Name: 	Kai Strubel
*     						Ben Ng			xbenng@gmail.com
*
*     Function Return Type: int
*
*     Parameters (list data type, name, and comment one per line):
*       1.Pedalbox_msg_t msg
			brake_level from pedalbox potentiometer
*			throttle_level from pedalbox potentiometer
*			APPS_Implausible flag
*			EOR flag
*		2.
*
*      Global Dependents:
*
*     Function Description:
*			Takes message from pedal box, runs safetly check, sets throttle.
*			Designed this way so pedalboxmsg's can be generated multiple ways,
*			and not disrupt the logic behind processing the data.
***************************************************************************/

	Pedalbox_msg_t pedalboxmsg;		//struct to store pedalbox msg
	while (1) {

		if(xQueueReceive(car.q_pedalboxmsg, &pedalboxmsg, 1000))
		{
			//get current time in ms
			uint32_t current_time_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;;
			// update time stamp, indicates when a pedalbox message was last received
			car.pb_msg_rx_time = current_time_ms;

			//check if calibration values should be updated
			if (car.calibrate_flag == CALIBRATE_THROTTLE_MIN) {
				car.throttle1_min = pedalboxmsg.throttle1_raw;
				car.throttle2_min = pedalboxmsg.throttle2_raw;
				car.calibrate_flag = CALIBRATE_NONE;
			} else if (car.calibrate_flag == CALIBRATE_THROTTLE_MAX) {
				car.throttle1_max = pedalboxmsg.throttle1_raw;
				car.throttle2_max = pedalboxmsg.throttle2_raw;
				car.calibrate_flag = CALIBRATE_NONE;
			} else 	if (car.calibrate_flag == CALIBRATE_BRAKE_MIN) {
				car.brake1_min = pedalboxmsg.brake1_raw;
				car.brake2_min = pedalboxmsg.brake2_raw;
				car.calibrate_flag = CALIBRATE_NONE;
			} else if (car.calibrate_flag == CALIBRATE_BRAKE_MAX) {
				car.brake1_max = pedalboxmsg.brake1_raw;
				car.brake2_max = pedalboxmsg.brake2_raw;
				car.calibrate_flag = CALIBRATE_NONE;
			}else{

			}

			/////////////PROCESS DATA///////////////
			int throttle1_sign = (car.throttle1_max - car.throttle1_min) / fabs(car.throttle1_max - car.throttle1_min);
			int throttle2_sign = (car.throttle2_max - car.throttle2_min) / fabs(car.throttle2_max - car.throttle2_min);
			//int throttle1_sign = -1;
			//int throttle2_sign = -1;

			float			throttle1_cal = ((float)(pedalboxmsg.throttle1_raw - car.throttle1_min)) / (car.throttle1_max - car.throttle1_min);  //value 0-1, throttle 1 calibrated between min and max
			float			throttle2_cal = ((float)(pedalboxmsg.throttle2_raw - car.throttle2_min)) / (car.throttle2_max - car.throttle2_min);;  //value 0-1, throttle 2 calibrated between min and max
			float 			brake1_cal	  = ((float)(pedalboxmsg.brake1_raw - car.brake1_min)) / (car.brake1_max - car.brake1_min);  //value 0-1, brake 1 calibrated between min and max
			float 			brake2_cal	  = ((float)(pedalboxmsg.brake2_raw - car.brake2_min)) / (car.brake2_max - car.brake2_min);  //value 0-1, brake 2 calibrated between min and max
			float 			throttle_avg  = (throttle1_cal + throttle2_cal) / 2.0;
			float			brake_avg     = (brake1_cal + brake2_cal) / 2.0;


			// EV 2.4.6: Encoder out of range
			if (
//				!(throttle1_sign * pedalboxmsg.throttle1_raw >= throttle1_sign * car.throttle1_min &&
//				 throttle1_sign * pedalboxmsg.throttle1_raw <= throttle1_sign * car.throttle1_max)
//				||
//				!(throttle2_sign * pedalboxmsg.throttle2_raw >= throttle2_sign * car.throttle2_min &&
//				throttle2_sign * pedalboxmsg.throttle2_raw <= throttle2_sign * car.throttle2_max)
				pedalboxmsg.throttle1_raw >= 0x0e70 ||
				pedalboxmsg.throttle1_raw <= 0x019a ||
				pedalboxmsg.throttle2_raw >= 0x0e70 ||
				pedalboxmsg.throttle2_raw <= 0x019a
			)
			{
				car.apps_state_eor = PEDALBOX_STATUS_ERROR;
			} else {
				car.apps_state_eor = PEDALBOX_STATUS_NO_ERROR;
			}

			//APPS Implausibility error handling, EV 2.3.5,
			//	error if throttle sensors disagree more than 10% travel
			//	for more than 100ms
			if (fabs(throttle1_cal - throttle2_cal) > .1 ) //legacy: (pedaboxmsg.APPS_Implausible == PEDALBOX_STATUS_ERROR)
			{
				//if error is persistent
				if (car.apps_state_imp == PEDALBOX_STATUS_ERROR_APPSIMP_PREV)  //<<is this a typo
				{
					//if time between first error and this error >= 100ms
					if (car.apps_imp_first_time_ms - current_time_ms >= 100)
					{
						car.apps_state_imp = PEDALBOX_STATUS_ERROR;
					}
				} else {  //else this is the first message to have an imp error
					//record the time
					car.apps_state_imp = PEDALBOX_STATUS_ERROR_APPSIMP_PREV;
					car.apps_imp_first_time_ms = current_time_ms;
				}
			} else {
				car.apps_state_imp = PEDALBOX_STATUS_NO_ERROR;

			}




//			BRAKE PLAUSIBILITY check

			if (throttle_avg >= .25 && brake_avg >= BRAKE_PRESSED_THRESHOLD)
			{
				car.apps_state_bp_plaus = PEDALBOX_STATUS_ERROR;
			}
			else if (throttle_avg <= APPS_BP_PLAUS_RESET_THRESHOLD) {//latch until this condition
				//EV 2.5.1, reset apps-brake pedal plausibility error only if throttle level is less than the .05
					car.apps_state_bp_plaus = PEDALBOX_STATUS_NO_ERROR;
			}


// set car variables
			car.brake = brake_avg;
			if (throttle_avg >= 0.1)
			{
				if (throttle_avg >= 0.9)
				{
					car.throttle_acc = MAX_THROTTLE_LEVEL;
				}
				else
				{
					//no errors, set throttle to value received from pedalbox
					car.throttle_acc = ((throttle_avg - 0.1) * MAX_THROTTLE_LEVEL / 0.8);
				}
			//car.throttle_cnt ++;
			}
			else
			{
				car.throttle_acc = 0;
			}

		}
	}

	//if this task breaks from the loop kill it
	vTaskDelete(NULL);
}

//int storeToFlash() {
//	HAL_FLASH_Unlock();
//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
//)
//};


