/*
 * CANRXProcess.h
 *
 *  Created on: Dec 22, 2016
 *      Author: ben
 */

#ifndef CANPROCESS_H_
#define CANPROCESS_H_

//includes
#include "motor_controller_functions.h"
//#include "WheelModule.h"

//defines for reading data from RxCanMsgTypeDef
#define ID_PEDALBOX1							0x500
#define ID_PEDALBOX2							0x501
#define	ID_PEDALBOXCALIBRATE					0x503
#define ID_BAMOCAR_STATION_TX					0x210	//message recieved by MC
#define ID_BAMOCAR_STATION_RX					0x190	//message sent by MC
//#define ID_BMS_PACK_VOLTAGE						0x400
#define ID_WHEEL_FR								0x100	// wheel module IDs
#define ID_WHEEL_FL								0x101
#define ID_WHEEL_RR								0x102
#define ID_WHEEL_RL								0x103
#define ID_DASHBOARD							0x350
#define ID_BMS_PACK_CUR_VOL						0x03B //current and voltage
#define ID_BMS_DCL								0x03C
#define ID_PEDALBOX_ERRORS						0x601

//wheel module defines
#define WM_SPEED_7_0_BYTE						2
#define WM_SPEED_11_8_BYTE						1
#define WM_SPEED_11_8_MASK						0x0F00




//pedalbox defines //todo not sure if better to send whole frame or just pbmsg.
#define PEDALBOX1_FILTER 						0	//filter number corresponding to the PEDALBOX1 message
#define PEDALBOX1_THROT1_7_0_BYTE				1
#define PEDALBOX1_THROT1_7_0_OFFSET				0
#define PEDALBOX1_THROT1_7_0_MASK				0b11111111
#define PEDALBOX1_THROT1_11_8_BYTE				0
#define PEDALBOX1_THROT1_11_8_OFFSET			0
#define PEDALBOX1_THROT1_11_8_MASK				0b00001111
#define PEDALBOX1_THROT2_7_0_BYTE				3
#define PEDALBOX1_THROT2_7_0_OFFSET				0
#define PEDALBOX1_THROT2_7_0_MASK				0b11111111
#define PEDALBOX1_THROT2_11_8_BYTE				2
#define PEDALBOX1_THROT2_11_8_OFFSET				0
#define PEDALBOX1_THROT2_11_8_MASK				0b00001111
//brake
#define PEDALBOX1_BRAKE1_7_0_BYTE				5
#define PEDALBOX1_BRAKE1_7_0_OFFSET				0
#define PEDALBOX1_BRAKE1_7_0_MASK				0b11111111
#define PEDALBOX1_BRAKE1_11_8_BYTE				4
#define PEDALBOX1_BRAKE1_11_8_OFFSET			0
#define PEDALBOX1_BRAKE1_11_8_MASK				0b00001111
#define PEDALBOX1_BRAKE2_7_0_BYTE				7
#define PEDALBOX1_BRAKE2_7_0_OFFSET				0
#define PEDALBOX1_BRAKE2_7_0_MASK				0b11111111
#define PEDALBOX1_BRAKE2_11_8_BYTE				6
#define PEDALBOX1_BRAKE2_11_8_OFFSET			0
#define PEDALBOX1_BRAKE2_11_8_MASK				0b00001111

#define PEDALBOX1_EOR_BYTE						3
#define PEDALBOX1_EOR_OFFSET					0
#define PEDALBOX1_EOR_MASK						0b00000001
#define PEDALBOX1_IMP_BYTE						3
#define PEDALBOX1_IMP_OFFSET					1
#define PEDALBOX1_IMP_MASK						0b00000010

void ISR_RXCAN();
void CANFilterConfig();
void taskRXCANProcess();
void taskTXCAN();
void taskRXCAN();
void processBamoCar(CanRxMsgTypeDef* rx);
void processWheelModuleFrame(CanRxMsgTypeDef* rx);
void processPedalboxFrame(CanRxMsgTypeDef* rx);


void processCalibrate(CanRxMsgTypeDef* rx);

#endif /* CANPROCESS_H_ */
