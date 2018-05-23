
#ifndef BMS_H_
#define BMS_H_

//#include "stm32f4xx_hal.h"

#define ID_BMS_AVGCELLVOLTAGE   0x401
#define ID_BMS_HIGHCELLVOLTAGE  0x402
#define ID_BMS_LOWCELLVOLTAGE   0x403
#define ID_BMS_AVGTemperature    0x404
#define ID_BMS_HIGHTemperature   0x405
#define ID_BMS_LOWTemperature    0x406
#define ID_BMS_PACKCURRENT      0x407
#define ID_BMS_PACKINSTVOLTAGE  0x408


#define AVG_CELLVOLTAGE_BITS_0_7    0
#define AVG_CELLVOLTAGE_BITS_11_8   1
#define HIGH_CELLVOLTAGE_BITS_0_7   0
#define HIGH_CELLVOLTAGE_BITS_11_8  1
#define LOW_CELLVOLTAGE_BITS_0_7    0
#define LOW_CELLVOLTAGE_BITS_11_8   1
#define AVG_Temperature_BITS_0_7    0
#define AVG_Temperature_BITS_11_8   1
#define HIGH_Temperature_BITS_0_7   0
#define HIGH_Temperature_BITS_11_8  1
#define LOW_Temperature_BITS_0_7    0 
#define LOW_Temperature_BITS_11_8   1
#define PACK_CURRENT_BITS_0_7       0
#define PACK_CURRENT_BITS_11_8      1
#define PACK_INST_VOLTAGE_BITS_0_7  0
#define PACK_INST_VOTLAGE_BITS_11_8 1


typedef struct {
	double avgCellVoltage;
	double highCellVoltage;
	double lowCellVoltage;
	uint16_t avgTemperature;
	uint16_t highTemperature;
	uint16_t lowTemperature;
	double packCurrent;
	double packInstVoltage;
	
	uint32_t avgCellvoltageLRTime;
	uint32_t highCellvoltageLRTime;
	uint32_t lowCellvoltageLRTime;
	uint32_t avgTemperatureLRTime;
	uint32_t highTemperatureLRTime;
	uint32_t lowTemperatureLRTime;
	uint32_t packCurrentLRTime;
	
} BMS_t;

extern BMS_t bms;

#endif /* BMS_H_ */
