/*
 * accelerometer.h
 *
 *  Created on: Feb 2, 2018
 *      Author: ben
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include "stm32f4xx_hal.h"



extern SPI_HandleTypeDef hspi1;
uint8_t TxBuff[2];
uint8_t RxBuff[2];
float Sensitivity;


#define X_L_ADDR					0x28
#define X_H_ADDR					0x29
#define Y_L_ADDR					0x2A
#define Y_H_ADDR					0x2B
#define Z_L_ADDR					0x2C
#define Z_H_ADDR					0x2D

#define CTRL_REG_4					0x20
#define ENABLE_XYZ					0x67
#define CTRL_REG_5					0x24



#define MEMS_PIN					GPIO_PIN_3
#define MEMS_PORT					GPIOE


//LEDs
#define RED_LED 					LD5_Pin
#define ORANGE_LED					LD3_Pin
#define BLUE_LED 					LD6_Pin
#define GREEN_LED 					LD4_Pin
#define LED_PORT 					GPIOD


typedef enum{
	Filter_800Hz,
	Filter_400Hz,
	Filter_200Hz,
	Filter_50Hz,

}Filter_t;

typedef enum {
	Sensitivity_2G,
	Sensitivity_4G,
	Sensitivity_6G,
	Sensitivity_8G,
	Sensitivity_16G,

}Sensitivity_t;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} Acceldata_t;


void Accelero_Init(Sensitivity_t Sens);
void Read_Axis_Data(uint8_t address, uint8_t* data);
void Accelro_Read_Axes();

#endif /* ACCELEROMETER_H_ */
