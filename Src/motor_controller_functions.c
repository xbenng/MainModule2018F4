/***************************************************************************
*
*     File Information
*
*     Name of File: main_module_tasks.c
*
*     Authors (Include Email):
*       1. Ben Ng				xbenng@gmail.com
*
*     File dependents: (header files, flow charts, referenced documentation)
*       1. motor_controller_functions.h
*       http://www.unitek-online.de/pdf/download/Antriebe-Drive/Servo-Digital/E-DS-CAN.pdf
*     	http://www.unitek-online.de/pdf/download/Antriebe-Drive/Servo-Digital/E-DS-NDrive.pdf
*
*
*     File Description:
*     	Functions to control the motor controller.

***************************************************************************/
#include "car.h"
#include "motor_controller_functions.h"
#include "CANProcess.h"

int taskProcessMotorControllerFrame() {
	CanRxMsgTypeDef rx;

	while (1) {
		if(xQueueReceive(car.q_mc_frame, &rx, 100))
		{
			//received a message from motor controller.
			//todo interpret frame
		}
	}
	return 0;
}

void mcCmdTorque(uint16_t torqueVal) {
	//example 5, BAMOCAR CAN MANUAL
	CanTxMsgTypeDef tx;
	tx.IDE = 		CAN_ID_STD;
	tx.StdId = 		ID_BAMOCAR_STATION_TX;
	tx.DLC = 		DLC_CMD_TORQUE;
	tx.RTR =		CAN_RTR_DATA;
	tx.Data[0] = 	REGID_CMD_TORQUE;
	tx.Data[1] =	(uint8_t) torqueVal;	//bytes 7-0
	tx.Data[2] =	(uint8_t) (torqueVal >> 8);		//bytes 11-8
	if (xSemaphoreTake(car.m_CAN, 100) == pdTRUE)
	{
		car.phcan->pTxMsg = &tx;
		HAL_CAN_Transmit(car.phcan, 100);
		xSemaphoreGive(car.m_CAN);  //release CAN mutex
	}
	//xQueueSendToBack(car.q_txcan, &tx, 100);
}

void mcCmdTransmissionRequestPermenant (uint8_t regid, uint8_t retransmitTimeMS) {
	//example 10, BAMOCAR CAN MANUAL
	CanTxMsgTypeDef tx;
	tx.IDE = 		CAN_ID_STD;
	tx.StdId = 		ID_BAMOCAR_STATION_TX;
	tx.DLC = 		DLC_CMD_REQUEST_DATA;
	tx.Data[0] = 	REGID_CMD_REQUEST_DATA;
	tx.Data[1] =	regid;
	tx.Data[2] =	(uint8_t) retransmitTimeMS;
	if (xSemaphoreTake(car.m_CAN, 100) == pdTRUE)
	{
		car.phcan->pTxMsg = &tx;
		HAL_CAN_Transmit(car.phcan, 100);
		xSemaphoreGive(car.m_CAN);  //release CAN mutex
	}
}

//use regid's defined in motor_controller.h
void mcCmdTransmissionRequestSingle(uint8_t regid) {
	//example 10, BAMOCAR CAN MANUAL
	CanTxMsgTypeDef tx;
	tx.IDE = 		CAN_ID_STD;
	tx.StdId = 		ID_BAMOCAR_STATION_TX;
	tx.DLC = 		DLC_CMD_REQUEST_DATA;
	tx.Data[0] = 	REGID_CMD_REQUEST_DATA;
	tx.Data[1] =	regid;
	tx.Data[2] =	RETRANSMISSION_SINGLE;
	xQueueSendToBack(car.q_txcan, &tx, 100);
}

void mcCmdTransmissionAbortPermenant(uint8_t regid) {
	//example 10, BAMOCAR CAN MANUAL
	CanTxMsgTypeDef tx;
	tx.IDE = 		CAN_ID_STD;
	tx.StdId = 		ID_BAMOCAR_STATION_TX;
	tx.DLC = 		DLC_CMD_REQUEST_DATA;
	tx.Data[0] = 	REGID_CMD_REQUEST_DATA;
	tx.Data[1] =	regid;
	tx.Data[2] =	RETRANSMISSION_ABORT;
	xQueueSendToBack(car.q_txcan, &tx, 100);

}

void disableMotor()
/***************************************************************************
*
*     Function Information
*
*     Name of Function: disableMotor
*
*     Programmer's Name: Ben Ng
*
*     Function Return Type: void
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
*			sends 0 torque, then disables RUN, and REF
***************************************************************************/
{
	//mcCmdTorque(0);
	HAL_GPIO_WritePin(FRG_RUN_GPIO_Port, FRG_RUN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RFE_GPIO_Port, RFE_Pin, GPIO_PIN_RESET);

}

void enableMotorController() {
/***************************************************************************
*
*     Function Information
*
*     Name of Function: initMotorController
*
*     Programmer's Name: Ben Ng
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.
*
*      Global Dependents:
*
*     Function Description:
*		Initializes the motor controller
*		chapter 10 bamocar ndrive manual
*
***************************************************************************/

	HAL_GPIO_WritePin(RFE_GPIO_Port, RFE_Pin, GPIO_PIN_SET);

	vTaskDelay(500 / portTICK_RATE_MS);  //wait 500ms, see BAMOCAR manual
	//request 10, BAMOCAR CAN MANUAL

	HAL_GPIO_WritePin(FRG_RUN_GPIO_Port, FRG_RUN_Pin, GPIO_PIN_SET);

	//Enable Motor Controller CAN Timeout
	CanTxMsgTypeDef tx;
	tx.IDE = 		CAN_ID_STD;
	tx.StdId = 		ID_BAMOCAR_STATION_TX;
	tx.DLC = 		3;
	tx.Data[0] = 	0xd0;
	tx.Data[0] = 	0x4f;
	tx.Data[1] =	0x01;
	//xQueueSendToBack(car.q_txcan, &tx, 100);
	if (xSemaphoreTake(car.m_CAN, 100) == pdTRUE)
	{
		car.phcan->pTxMsg = &tx;
		HAL_CAN_Transmit(car.phcan, 100);
		xSemaphoreGive(car.m_CAN);  //release CAN mutex
	}



}


void taskMotorControllerPoll(void* param)
/***************************************************************************
*
*     Function Information
*
*     Name of Function: taskMotorControllerPoll
*
*     Programmer's Name: Ben Ng
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1. none
*
*     Global Dependents:
*
*     Function Description:
*		This task requsts frames from the motor controller at a constant interval
*		it does this without verifying that the frames have been sent back
*		consider doing this with verification and a read requested flag
*
***************************************************************************/
{
	/*
	int numStates = 2;
	int state = 0;
	for (;;)
	{
		if (state > numStates)
		{
			state = 0;
		}
		switch (state)
		{
		// the motor controller frames to request
			case 0:
				mcCmdTransmissionRequestSingle(REGID_SPEED_ACTUAL_FILTER);
			case 1:
				mcCmdTransmissionRequestSingle(REGID_SPEED_ACTUAL_FILTER);
		}
		state++;
		if (50/state > 20)
		{
			vTaskDelay(50/state);
		} else {
			vTaskDelay(20);
		}
	}*/
	while(1)
		{
			//0x03B
			//length 5
			//byte0 - Pack Current - 2 bytes - MSB First - Big Endian - 0.1A
			//byte1 - IN USE - 1 bytes - MSB First
			//byte2 - Pack Inst. Voltage - 2 bytes - MSB First - Big Endian - * (1/10) - 0.1V
			//byte3 - IN USE - 1 bytes - MSB First
			//byte4 - CRC Checksum - 1 bytes - MSB First  - +64
			//0x03C
			//length 2
			//byte0 - Pack DCL - 2 bytes - MSB First - Big Endian - 1A
			//byte1 - IN USE - 1bytes - MSB First
			// Request Parameters
//
//				BCparam = 0;			// BCparam 0 - Nothing received
//				while(BCparam != 1) {
//					mcCmdTransmissionRequestSingle(REGID_I_ACT);
//					vTaskDelay(POLL_DELAY);
//				}	// BCparam 1 - actualTorque received
//				while(BCparam != 2) {
//					mcCmdTransmissionRequestSingle(ID_BMS_PACK_CUR_VOL);
//					vTaskDelay(POLL_DELAY);
//				}	// BCparam 2 - actualDC received
//				while(BCparam != 3) {
//					mcCmdTransmissionRequestSingle(ID_BMS_DCL);
//					vTaskDelay(POLL_DELAY);
//				}	// BCparam 3 - DCLimit received
		BCparam = 0;            // BCparam 0 - Nothing received
		            while(BCparam != 1) {
		                mcCmdTransmissionRequestSingle(REGID_I_ACT);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 1 - actualTorque received
		            while(BCparam != 2) {
		                mcCmdTransmissionRequestSingle(REGID_SPEED_ACTUAL);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 2 - speed actual received
		            while(BCparam != 3) {
		                mcCmdTransmissionRequestSingle(REGID_I_IST);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 3 - current actual received
		            while(BCparam != 4) {
		                mcCmdTransmissionRequestSingle(REGID_I_SOLL);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 4 - command current received
		            while(BCparam != 5) {
		                mcCmdTransmissionRequestSingle(REGID_DC_BUS);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 5 - current actual received
		            while(BCparam != 6) {
		                mcCmdTransmissionRequestSingle(REGID_T_MOTOR);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 6 - motor temperature received
		            while(BCparam != 7) {
		                mcCmdTransmissionRequestSingle(REGID_T_IGBT);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 7 - power stage temperature received
		            while(BCparam != 8) {
		                mcCmdTransmissionRequestSingle(REGID_T_AIR);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 8 - air temperature received
		            while(BCparam != 9) {
		                mcCmdTransmissionRequestSingle(REGID_I_REDA);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 9 - actual current limit received
		            while(BCparam != 10) {
		                mcCmdTransmissionRequestSingle(REGID_ERR_BITMAP1);
		                vTaskDelay(POLL_DELAY);
		            }    // BCparam 10 - errBitMap1 received

		}
}


