#include <stdint.h>
#include <stdbool.h>
#include "MAX30100_Registers.h"
#include "I2C.c"


#define DEFAULT_MODE                MAX30100_MODE_HRONLY
#define DEFAULT_SAMPLING_RATE       MAX30100_SAMPRATE_100HZ
#define DEFAULT_PULSE_WIDTH         MAX30100_SPC_PW_1600US_16BITS
#define DEFAULT_RED_LED_CURRENT     MAX30100_LED_CURR_50MA
#define DEFAULT_IR_LED_CURRENT      MAX30100_LED_CURR_50MA
#define EXPECTED_PART_ID            0x11

bool begin()
{
    if(getPartId() == EXPECTED_PART_ID)
    	return false;

    //setMode(DEFAULT_MODE);
    //setLedsPulseWidth(DEFAULT_PULSE_WIDTH);
    //setSamplingRate(DEFAULT_SAMPLING_RATE);
    //setLedsCurrent(DEFAULT_IR_LED_CURRENT, DEFAULT_RED_LED_CURRENT);
    //setHighresModeEnabled(true);    
}

uint8_t getPartId()
{
	uint8_t partId [1] = {0};
	I2C_Master_ReadReg(SLAVE_ADDR, MAX30100_REG_PART_ID, 1);
	CopyArray(ReceiveBuffer, partId, 1);
	return partId[0];
}
