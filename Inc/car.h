/*
 * car.h
 *
 *  Created on: Jan 3, 2017
 *      Author: ben
 */

#ifndef CAR_H_
#define CAR_H_

#include "main.h"
#include "cmsis_os.h"
#include "PedalBox.h"
#include "stm32f4xx_hal.h"
#include "CANProcess.h"
#include "BMS.h"
#include "motor_controller_functions.h"
//#include "accelerometer.h"
#include <math.h>



////gpio aliases
//#define BUZZER_PORT			GPIOB	//todo
//#define BUZZER_PIN			GPIO_PIN_10
//#define FRG_RUN_PORT		GPIOE
//#define FRG_RUN_PIN			GPIO_PIN_11
//#define RFE_PORT			GPIOE
//#define RFE_PIN				GPIO_PIN_12
//#define BRAKE_LIGHT_PORT	GPIOE
//#define BRAKE_LIGHT_PIN		GPIO_PIN_7
//#define HEARTBEAT_PORT		GPIOE
//#define HEARTBEAT_PIN		GPIO_PIN_1


#define BRAKE_PRESSED_THRESHOLD	.1
#define APPS_BP_PLAUS_RESET_THRESHOLD .05  //EV 2.5
#define APPS_BP_PLAUS_THRESHOLD .25  //EV 2.5

#define PERIOD_ACCELRO				50 / portTICK_RATE_MS
#define PERIOD_TORQUE_SEND		 	100 / portTICK_RATE_MS
#define HEARTBEAT_PULSEWIDTH		200 / portTICK_RATE_MS
#define HEARTBEAT_PERIOD			100 / portTICK_RATE_MS
#define PEDALBOX_TIMEOUT			1000 / portTICK_RATE_MS
#define POLL_DELAY					50 / portTICK_RATE_MS
#define MAX_BRAKE_LEVEL 			0xFFF
#define MAX_THROTTLE_LEVEL			0x7FFE
#define LC_THRESHOLD				10			// todo lc threshold DUMMY VALUE
#define LAUNCH_CONTROL_INTERVAL_MS	10


//rtos parameter defines
#define QUEUE_SIZE_RXCAN			16
#define QUEUE_SIZE_PEDALBOXMSG		16
#define QUEUE_SIZE_TXCAN			3
#define QUEUE_SIZE_MCFRAME			3


typedef enum
{
	BRAKE_LIGHT_OFF = GPIO_PIN_RESET,
  	BRAKE_LIGHT_ON = GPIO_PIN_SET
} Brake_light_status_t;

typedef enum
{
	PC_INPROGRESS = GPIO_PIN_SET,
	PC_COMPLETE = GPIO_PIN_RESET
} PC_status_t;

//launch control
typedef enum {
	LC_ACTIVATED,
	LC_DISABLED
} LC_status_t;

typedef enum {
	CAR_STATE_INIT,
	CAR_STATE_PREREADY2DRIVE,
	CAR_STATE_READY2DRIVE,
	CAR_STATE_ERROR,
	CAR_STATE_RESET
} Car_state_t;

typedef enum {
	PEDALBOX_MODE_ANALOG,
	PEDALBOX_MODE_DIGITAL
} Pedalbox_mode_t;

typedef enum {
	CALIBRATE_NONE,
	CALIBRATE_THROTTLE_MIN,
	CALIBRATE_THROTTLE_MAX,
	CALIBRATE_BRAKE_MIN,
	CALIBRATE_BRAKE_MAX
} Calibrate_flag_t;


int BCparam;
int actualTorque0700;
int actualTorque1508;
int actualDC;
int actualV;
int DCLimit;
int calcTorqueLimit;
int pedalTorque;
int actualTorque;
int torque_to_send;
int speedActual;
int currentActual;
int commandCurrent;
int dcBusVoltage;
int motorTemperature;
int powerStageTemperature;
int airTemperature;
int actualCurrentLimit;
int errBitMap1;

typedef struct {

	Car_state_t 			state;
	uint8_t					errorFlags;
	//calibration values
	int32_t				throttle1_min; //this is a higher value than max
	int32_t				throttle1_max; //this is a lower value than min
	int32_t				throttle2_min;
	int32_t				throttle2_max;
	int32_t				brake1_min;
	int32_t				brake1_max;
	int32_t				brake2_min;
	int32_t				brake2_max;
	int64_t 				throttle_acc;				//sum of car's intended throttle messages from pedalbox since last cmd sent to MC
	int16_t					throttle_cnt;				//number of throttle messages in accumulator
	float 				brake;						//car's intended brake position
	uint32_t				pb_msg_rx_time;				//indicates when a pedalbox message was last received
	uint32_t				apps_imp_first_time_ms;		//indicates when the first imp error was received
	Pedalbox_status_t		apps_state_imp;		//the last pedalbox message imp sate
	Pedalbox_status_t		apps_state_bp_plaus;				//apps-brake plausibility status
	Pedalbox_status_t		apps_state_eor;				//apps-brake plausibility status
	Pedalbox_status_t		apps_state_timeout;				//apps-brake plausibility status
	Pedalbox_mode_t			pb_mode;					//determines whether pb will be analog or CAN
	Calibrate_flag_t		calibrate_flag;

	LC_status_t				lc_status;
	//Pedalbox_msg_t 			pb_current_msg;

	//RTOS objects, initialized in initRTOSObjects
	QueueHandle_t			q_rxcan;
	QueueHandle_t			q_txcan;
	QueueHandle_t	 		q_pedalboxmsg;
	QueueHandle_t			q_mc_frame;

	SemaphoreHandle_t		m_CAN;						//mutex for CAN peripheral

	CAN_HandleTypeDef *		phcan;						//pointer to car's CAN peripheral handle

} Car_t;

extern volatile Car_t car;
extern CAN_HandleTypeDef hcan1;

//function prototypes
void carSetBrakeLight(Brake_light_status_t status);
void ISR_StartButtonPressed();
void carInit();
void taskPedalBoxMsgHandler();
void taskCarMainRoutine();
int SendTorqueTask();
int mainModuleWatchdogTask();
int taskHeartbeat();
void taskSoundBuzzer(int* time);
void initRTOSObjects();
void taskBlink(void* can);
void stopCar();
void taskSendAccelero();
void taskMotorControllerPoll();


#endif /* CAR_H_ */
