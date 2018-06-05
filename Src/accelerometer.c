/*
 * accelerometer.c
 *
 *  Created on: Feb 2, 2018
 *      Author: ben
 */
#include "accelerometer.h"


//Initializes accelerometer to use 100hz frequency and enables XYZ AXES
void Accelero_Init(Sensitivity_t Sens){

	//Register to be written to
	TxBuff[0] = CTRL_REG_4;
	//Default to enable XYZ and 2G Sensitivity
	TxBuff[1] = ENABLE_XYZ;	//(0x67)

	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_RESET);

		  //sends configurations to MEMS
	HAL_SPI_Transmit(&hspi1, TxBuff, 2, 50);

		  //SPI1 CS high for end transmission
	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_SET);

	TxBuff[0] = CTRL_REG_5;

	switch (Sens){

	case (Sensitivity_2G):
			TxBuff[1] = 0x40;
			Sensitivity = 0.06;
			break;

	case (Sensitivity_4G):
			TxBuff[1] = 0x48;
			Sensitivity = 0.12;
			break;

	case (Sensitivity_6G):
			TxBuff[1] = 0x50;
			Sensitivity = 0.18;
			break;

	case (Sensitivity_8G):
			TxBuff[1] |= 0x58;
			Sensitivity = 0.24;
			break;

	case(Sensitivity_16G):
			TxBuff[1] |= 0x60;
			Sensitivity = 0.73;
			break;
	}

	//SPI1 CS low for transmission

	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_RESET);

		  //sends configurations to MEMS
	HAL_SPI_Transmit(&hspi1, TxBuff, 2, 50);

		  //SPI1 CS high for end transmission
	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_SET);

	  //flashes lights to show that board is done with INIT
//	  for (int i = 0; i < 5; i++){
//	 		  HAL_GPIO_TogglePin(LED_PORT, RED_LED | GREEN_LED | ORANGE_LED | BLUE_LED);
//	 		  HAL_Delay(200);
//	 	  }

}

void Accelro_Read_Axes(Acceldata_t* data){

	uint8_t buffer[6];

	//gets most significant bit and least significant bit and adds them to buffer array
	Read_Axis_Data(X_L_ADDR, &buffer[0]);
	Read_Axis_Data(X_H_ADDR, &buffer[1]);
	Read_Axis_Data(Y_L_ADDR, &buffer[2]);
	Read_Axis_Data(Y_H_ADDR, &buffer[3]);
	Read_Axis_Data(Z_L_ADDR, &buffer[4]);
	Read_Axis_Data(Z_H_ADDR, &buffer[5]);

	//converts LSB and MSB to 16 bit int and multiplies by sensitivity selection
	data->x = (int16_t)((buffer[1] << 8) + buffer[0]) * Sensitivity;
	data->y = (int16_t)((buffer[3] << 8) + buffer[2]) * Sensitivity;
	data->z = (int16_t)((buffer[5] << 8) + buffer[4]) * Sensitivity;

}

void Read_Axis_Data(uint8_t address, uint8_t *data){

	TxBuff[0] = address | 0x80;

	//enable communication
	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_RESET);

	//send axis address and ready to receive bit
	HAL_SPI_Transmit(&hspi1, TxBuff, 1, 50);

	//receive data
	HAL_SPI_Receive(&hspi1, data, 1, 50);

	//disable communication
	HAL_GPIO_WritePin(MEMS_PORT, MEMS_PIN, GPIO_PIN_SET);
}

void taskSendAccelero() {
/***************************************************************************
*
*     Function Information
*
*     Name of Function: taskSend
*
*     Programmer's Name: Ben Ng
*
*     Function Return Type: void
*
*     Parameters (list data type, name, and comment one per line):
*       1.
*
*     Global Dependents:
*
*     Function Description:
*     polls accelerometer, and sends the acceleration data over CAN
*
***************************************************************************/
	while (1) {
		Acceldata_t adata;
		Accelro_Read_Axes(&adata);
		CanTxMsgTypeDef tx;
		tx.StdId = 0x420;
		tx.Data[0] = (adata.x >> 8) & 0xFF;
		tx.Data[1] = adata.x & 0xFF;
		tx.Data[2] = (adata.y >> 8) & 0xFF;
		tx.Data[3] = adata.y & 0xFF;
		tx.Data[4] = (adata.z >> 8) & 0xFF;
		tx.Data[5] = adata.z & 0xFF;
		tx.DLC = 6;
		tx.RTR = CAN_RTR_DATA;
		tx.IDE = CAN_ID_STD;
		//car.phcan->pTxMsg = &tx;
		//HAL_CAN_Transmit_IT(car.phcan);

		vTaskDelay(50);
	}
}
