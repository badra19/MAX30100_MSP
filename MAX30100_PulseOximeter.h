/*
 * MAX30100_PulseOximeter.h
 *
 *  Created on: 28 de mai de 2018
 *      Author: Davi Mendes
 */

#ifndef MAX30100_PULSEOXIMETER_H_
#define MAX30100_PULSEOXIMETER_H_

#define SAMPLING_FREQUENCY                  100
#define CURRENT_ADJUSTMENT_PERIOD_MS        500
#define DEFAULT_IR_LED_CURRENT              MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT_START               MAX30100_LED_CURR_27_1MA
#define DC_REMOVER_ALPHA                    0.95

#include <stdint.h>

#include "MAX30100.h"
//#include "MAX30100_BeatDetector.h"
#include "MAX30100_Filters.h"
//#include "MAX30100_SpO2Calculator.h"

typedef enum PulseOximeterState {
    PULSEOXIMETER_STATE_INIT,
    PULSEOXIMETER_STATE_IDLE,
    PULSEOXIMETER_STATE_DETECTING
} PulseOximeterState;

typedef enum PulseOximeterDebuggingMode {
    PULSEOXIMETER_DEBUGGINGMODE_NONE,
    PULSEOXIMETER_DEBUGGINGMODE_RAW_VALUES,
    PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES,
    PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT
} PulseOximeterDebuggingMode;

PulseOximeterDebuggingMode debuggingMode;


#endif /* MAX30100_PULSEOXIMETER_H_ */
