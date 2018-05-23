/***************************************************************************
*
*     File Information
*
*     Name of File: CANRXProcess.c
*
*     Authors (Include Email):
*       1. Ben Ng,       xbenng@gmail.com
*
*     File dependents: (header files, flow charts, referenced documentation)
*       1. FreeRTOS.h
* 		2. stm32f7xx_hal_can.h
* 		3. CANRXProcess.h
*
*     File Description: Used for interpreting incoming CAN messages on
*						main module
*
***************************************************************************/
#include <CANProcess.h>

#include "car.h"
#include "PedalBox.h"


/***************************************************************************
*
*     Function Information
*
*     Name of Function: RXCAN_ISR
*
*     Programmer's Name: Ben Ng, xbenng@gmail.com
*
*     Function Return Type:
*
*     Parameters (list data type, name, and comment one per line):
*
*     Global Dependents:
*	  1. QueueHandle_t	q_pedalbox_msg;
*
*     Function Description:
*			To be called by CAN1_RX0_IRQHandler in order to queue
*			received CAN messages to be processed by RXCANProcessTask
*
***************************************************************************/
void ISR_RXCAN()
{
}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: RXCANProcessTask
*
*     Programmer's Name: Ben Ng, xbenng@gmail.com
*
*     Function Return Type:
*
*     Parameters (list data type, name, and comment one per line):
*       1. CAN_HandleTypeDef *hcan, hcan structure address to add filter to
*
*     Global Dependents:
*	    1.
*
*     Function Description: Filter Configuration.
*
***************************************************************************/
//void CANFilterConfig()
//{
//	  CAN_FilterConfTypeDef filter_conf;  //filter config object
//
//	  //see filter configuration section of manifesto for filter numbering
//	  //also refer to "CAN Messages" in team documentation for addresses
//	  filter_conf.FilterIdHigh = 		0x7ff << 5; // 2
//	  filter_conf.FilterIdLow = 		ID_PEDALBOX1 << 5; // 0, "pedalbox1" throttle values and pedalbox errors
//	  filter_conf.FilterMaskIdHigh = 	0 << 5; //3
//	  filter_conf.FilterMaskIdLow = 	0 << 5;	//1, pedal errors
//	  filter_conf.FilterFIFOAssignment = CAN_FilterFIFO0;  //use interrupt RX0
//	  filter_conf.FilterNumber = 0;
//	  filter_conf.FilterMode = CAN_FILTERMODE_IDMASK;  //four different filters, no masks
//	  filter_conf.FilterScale = CAN_FILTERSCALE_16BIT; //16 bit filters
//	  filter_conf.FilterActivation = ENABLE;
//	  HAL_CAN_ConfigFilter(car.phcan, &filter_conf); //add filter
//}
void CANFilterConfig()
{


	  CAN_FilterConfTypeDef FilterConf;
	  FilterConf.FilterIdHigh =         ID_PEDALBOX2 << 5; // 2 num
	  FilterConf.FilterIdLow =          0x350 << 5; // 0
	  FilterConf.FilterMaskIdHigh =     0x7ff;       // 3
	  FilterConf.FilterMaskIdLow =      0x7fe;       // 1
//	  FilterConf.FilterIdHigh = 	0b0000000001000000; // 2
//	  FilterConf.FilterIdLow = 		ID_PEDALBOXCALIBRATE << 5; // 0
//	  FilterConf.FilterMaskIdHigh = ID_PEDALBOX2 << 5; //3
//	  FilterConf.FilterMaskIdLow = 	ID_DASHBOARD << 5;	//1
	  FilterConf.FilterFIFOAssignment = CAN_FilterFIFO0;
	  FilterConf.FilterNumber = 0;
	  FilterConf.FilterMode = CAN_FILTERMODE_IDLIST;
	  FilterConf.FilterScale = CAN_FILTERSCALE_16BIT;
	  FilterConf.FilterActivation = ENABLE;
	  HAL_CAN_ConfigFilter(car.phcan, &FilterConf);


}


/***************************************************************************
*
*     Function Information
*
*     Name of Function: taskTXCAN
*
*     Programmer's Name: Ben Ng, xbenng@gmail.com
*
*     Function Return Type: none
*
*     Parameters (list data type, name, and comment one per line):
*
*      Global Dependents:
*	   1.
*
*     Function Description:
*     	Task function to send CAN messages using the CAN peripheral
*
***************************************************************************/
void taskTXCAN()
{
	CanTxMsgTypeDef tx;

	for (;;)
	{
		//check if this task is triggered
		if (xQueuePeek(car.q_txcan, &tx, portMAX_DELAY) == pdTRUE)
		{
			//check if CAN mutex is available
			if (xSemaphoreTake(car.m_CAN, 50) == pdTRUE)
			{
				//HAL_CAN_StateTypeDef state = HAL_CAN_GetState(car.phcan);
				//if (state != HAL_CAN_STATE_ERROR)
				{
					xQueueReceive(car.q_txcan, &tx, portMAX_DELAY);  //actually take item out of queue
					car.phcan->pTxMsg = &tx;
					HAL_CAN_Transmit(car.phcan, 1000);
				}
				xSemaphoreGive(car.m_CAN);  //release CAN mutex
			}

		}
	}
}

//void taskRXCAN() /////////this should not be a thing.
//{
//	CanRxMsgTypeDef rx;
//	car.phcan->pRxMsg = &rx;
//	for (;;)
//	{
//
//		//check if CAN mutex is available
//		if (xSemaphoreTake(car.m_CAN, 10) == pdTRUE )
//		{
//			HAL_CAN_Receive_IT(car.phcan, 0);
//			HAL_CAN_Receive_IT(car.phcan, 1);
//			xSemaphoreGive(car.m_CAN);  //release CAN mutex
//		}
//		vTaskSuspend(NULL);
//	}
//}

/***************************************************************************
*
*     Function Information
*
*     Name of Function: taskRXCAN
*
*     Programmer's Name: Ben Ng, xbenng@gmail.com
*
*     Function Return Type: none
*
*     Parameters (list data type, name, and comment one per line):
*
*      Global Dependents:
*	   1.-
*
*     Function Description:
*     	Task function to process received CAN Messages.
*     	CanRxMsgTypeDef are sent here to the q_rxcan queue to be processed
*     	from the CAN RX interrupt handler.
*     	The data is process and handled according to what kind of message is received
*
***************************************************************************/
void taskRXCANProcess()
{

	CanRxMsgTypeDef rx;  //CanRxMsgTypeDef to be received on the queue
	while (1)
	{

		//if there is a CanRxMsgTypeDef in the queue, pop it, and store in rx
		if (xQueueReceive(car.q_rxcan, &rx, portMAX_DELAY) == pdTRUE)
		{
			//A CAN message has been recieved
			//HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
			//check what kind of message we received
			switch (rx.StdId)
			{
				case ID_PEDALBOX2:  //if pedalbox1 message
				{
					processPedalboxFrame(&rx);
					break;
				}
				case ID_PEDALBOXCALIBRATE: {
					processCalibrate(&rx);
					break;
				}
				case ID_BAMOCAR_STATION_RX: { //if bamocar message
					//forward frame to mc frame queue
					processBamoCar(&rx);
					//xQueueSend(car.q_mc_frame, &rx, 100);
				}
				case  	ID_WHEEL_FR:
				case	ID_WHEEL_FL:
				case	ID_WHEEL_RR:
				case	ID_WHEEL_RL: //todo add all the other wheel module IDs
				{
					//processWheelModuleFrame(&rx);
					break;
				}
				case	ID_DASHBOARD:
				{
					ISR_StartButtonPressed();
					break;
				}
				case ID_BMS_PACK_CUR_VOL:
					actualDC = rx.Data[1] | (rx.Data[1] << 8);
					actualV = rx.Data[3] | (rx.Data[2]<< 8);
			}
		}
	}
}

//void processWheelModuleFrame(CanRxMsgTypeDef* rx) {
//	uint16_t speed = 0;
//	speed |= (rx->Data[WM_SPEED_7_0_BYTE] & 0xFF);
//	speed |= ((rx->Data[WM_SPEED_11_8_BYTE] << 8) & 	0x0F00);
//	//todo process wheel module stuff
//	if (rx->StdId == ID_WHEEL_FR) {
//		wheelModule.speedFR = rx->Data[0];
//	}
//}

/***************************************************************************
*
*     Function Information
*
*     Name of Function: processPedalboxFrame
*
*     Programmer's Name: Ben Ng, xbenng@gmail.com
*
*     Function Return Type: none
*
*     Parameters (list data type, name, and comment one per line):
*		1. CanRxMsgTypeDef* rx, CAN frame to be converted into a pedalbox message
*      Global Dependents:
*	    1.
*
*     Function Description:
*     	Converts CAN frame into a pedalbox message and sends it to pedalboxmsg handler
*
***************************************************************************/
void processPedalboxFrame(CanRxMsgTypeDef* rx)
{
	if (car.pb_mode == PEDALBOX_MODE_DIGITAL)	//
	{
		Pedalbox_msg_t pedalboxmsg;

		///////////SCRUB DATA the from the CAN frame//////////////
		//mask then shift the throttle value data
		uint8_t throttle1_7_0 	=
				rx->Data[PEDALBOX1_THROT1_7_0_BYTE]  >> PEDALBOX1_THROT1_7_0_OFFSET;  //Throttle 1 Value (7:0) [7:0]
		uint8_t throttle1_11_8	=
				(rx->Data[PEDALBOX1_THROT1_11_8_BYTE] & PEDALBOX1_THROT1_11_8_MASK) >> PEDALBOX1_THROT1_11_8_OFFSET;  //Throttle 1 Value (11:8) [3:0]
		uint8_t throttle2_7_0	=
				rx->Data[PEDALBOX1_THROT2_7_0_BYTE]  >> PEDALBOX1_THROT2_7_0_OFFSET;  //Throttle 2 Value (7:0) [7:0]
		uint8_t throttle2_11_8	=
				(rx->Data[PEDALBOX1_THROT2_11_8_BYTE] & PEDALBOX1_THROT2_11_8_MASK) >> PEDALBOX1_THROT2_11_8_OFFSET;  //Throttle 2 Value (11:8) [3:0]

		//mask then shift the brake value data
		uint8_t brake1_7_0 	=
				rx->Data[PEDALBOX1_BRAKE1_7_0_BYTE]  >> PEDALBOX1_BRAKE1_7_0_OFFSET;  //brake 1 Value (7:0) [7:0]
		uint8_t brake1_11_8	=
				(rx->Data[PEDALBOX1_BRAKE1_11_8_BYTE] & PEDALBOX1_BRAKE1_11_8_MASK) >> PEDALBOX1_BRAKE1_11_8_OFFSET;  //brake 1 Value (11:8) [3:0]
		uint8_t brake2_7_0	=
				rx->Data[PEDALBOX1_BRAKE2_7_0_BYTE]  >> PEDALBOX1_BRAKE2_7_0_OFFSET;  //brake 2 Value (7:0) [7:0]
		uint8_t brake2_11_8	=
				(rx->Data[PEDALBOX1_BRAKE2_11_8_BYTE] & PEDALBOX1_BRAKE2_11_8_MASK) >> PEDALBOX1_BRAKE2_11_8_OFFSET;  //brake 2 Value (11:8) [3:0]

		//mask then shift the error flags //we nolonger calculate erros on PedalBox board. 10-23-2017
//		pedalboxmsg.APPS_Implausible =
//				(rx->Data[PEDALBOX1_IMP_BYTE] & PEDALBOX1_IMP_MASK) >> PEDALBOX1_IMP_OFFSET;
//		pedalboxmsg.EOR =
//				(rx->Data[PEDALBOX1_EOR_BYTE] & PEDALBOX1_EOR_MASK) >> PEDALBOX1_EOR_OFFSET;


		//build the data
		pedalboxmsg.throttle1_raw = 0;
		pedalboxmsg.throttle1_raw |= throttle1_7_0 << 0;
		pedalboxmsg.throttle1_raw |= throttle1_11_8 << 8;
		pedalboxmsg.throttle2_raw = 0;
		pedalboxmsg.throttle2_raw |= throttle2_7_0 << 0;
		pedalboxmsg.throttle2_raw |= throttle2_11_8 << 8;
		pedalboxmsg.brake1_raw = 0;
		pedalboxmsg.brake1_raw |= brake1_7_0 << 0;
		pedalboxmsg.brake1_raw |= brake1_11_8 << 8;
		pedalboxmsg.brake2_raw = 0;
		pedalboxmsg.brake2_raw |= brake2_7_0 << 0;
		pedalboxmsg.brake2_raw |= brake2_11_8 << 8;


		//send to pedalboxmsg to queue
		xQueueSendToBack(car.q_pedalboxmsg, &pedalboxmsg, 100);
	}
}

void processCalibrate(CanRxMsgTypeDef* rx) {
	//set the calibration flag, so calibration values are updated upon reception of new pedalboxmsg
	if 		  (rx->Data[0] == 0x01) {
		car.calibrate_flag = CALIBRATE_THROTTLE_MAX; //calibrate high
	} else if (rx->Data[0] == 0x02) {
		car.calibrate_flag = CALIBRATE_THROTTLE_MIN; //calibrate low
	} else if (rx->Data[0] == 0x03) {
		car.calibrate_flag = CALIBRATE_BRAKE_MAX; //calibrate high
	} else if (rx->Data[0] == 0x04) {
		car.calibrate_flag = CALIBRATE_BRAKE_MIN; //calibrate low
	} else {
		car.calibrate_flag = CALIBRATE_NONE;
	}

}
/***************************************************************************
*
*     Function Information
*
*     Name of Function: processBamoCar
*
*     Programmer's Name: Raymond Dong, raymonddonghome@gmail.com
*
*     Function Return Type: none
*
*     Parameters:
*		1. CanRxMsgTypeDef* rx, CAN frame to be converted into a bamocar message
*
*     Global Dependents:
*	    1. actualTorque
*		2. actualDC
*		3. DCLimit
*		4. pedalTorque
*
*     Function Description:
*     	Updates variables
*
***************************************************************************/
void processBamoCar(CanRxMsgTypeDef* rx)
{
	/*
	{actual torque}				{actual dc current}			{dc current limit}
	{calculated torque limit}	{pedal torque}
	{calculated torque limit} = {dc current limit}/{actual dc current} * {actual torque}
	if {calculated torque limit} > {pedal torque}
		send {calculated torque limit}
	else
		send {pedal torque}
	*/
//	if (rx->Data[0] == REGID_I_ACT)
//	{
//		actualTorque0700 = rx->Data[1];
//		actualTorque1508 = rx->Data[2];
//		actualTorque = actualTorque0700 | (actualTorque1508 << 8);
//		BCparam = 1;	//actualTorque received
//	}
//	if (rx->Data[0] == ID_BMS_PACK_CUR_VOL)
//	{
//
//		BCparam = 2;	//actualDc received
//	}
//	if (rx->Data[0] == ID_BMS_DCL)
//	{
//
//		BCparam = 3;	//DCLimit received
//	}
	if (rx->Data[0] == REGID_I_ACT)
	    {
	        actualTorque0700 = rx->Data[1];
	        actualTorque1508 = rx->Data[2];
	        actualTorque = actualTorque0700 | (actualTorque1508 << 8);
	        BCparam = 1;    //actual torque received
	    }
	    if (rx->Data[0] == REGID_SPEED_ACTUAL)
	    {
	        speedActual = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 2;    //speed actual received
	    }
	    if (rx->Data[0] == REGID_I_IST)
	    {
	        currentActual = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 3;    //current actual received
	    }
	    if (rx->Data[0] == REGID_I_SOLL)
	    {
	        commandCurrent = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 4;    //command current received
	    }
	    if (rx->Data[0] == REGID_DC_BUS)
	    {
	        dcBusVoltage = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 5;    //current actual received
	    }
	    if (rx->Data[0] == REGID_T_MOTOR)
	    {
	        motorTemperature = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 6;    //motor temperature received
	    }
	    if (rx->Data[0] == REGID_T_IGBT)
	    {
	        powerStageTemperature = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 7;    //power stage temperature received
	    }
	    if (rx->Data[0] == REGID_T_AIR)
	    {
	        airTemperature = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 8;    //air temperature received
	    }
	    if (rx->Data[0] == REGID_I_REDA)
	    {
	        actualCurrentLimit = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 9;    //actual current limit received
	    }
	    if (rx->Data[0] == REGID_ERR_BITMAP1)
	    {
	        errBitMap1 = (rx -> Data[1] << 8 | rx -> Data[2]);
	        BCparam = 10;    //errBitMap1 received
	    }
}
