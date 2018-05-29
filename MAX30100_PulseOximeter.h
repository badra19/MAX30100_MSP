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
#include "MAX30100_BeatDetector.h"
#include "MAX30100_Filters.h"
#include "MAX30100_SpO2Calculator.h"

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
PulseOximeterState state;
LEDCurrent irLedCurrent;
uint8_t redLedCurrentIndex;

uint32_t tsFirstBeatDetected;
uint32_t tsLastBeatDetected;
uint32_t tsLastBiasCheck;
uint32_t tsLastCurrentAdjustment;

void (*onBeatDetected)();
bool pulseOxBegin(PulseOximeterDebuggingMode debuggingMode_);
void pulseOxCheckSample();
void pulseOxCheckCurrentBias();
void pulseOxUpdate();
float pulseOxGetHeartRate();
uint8_t pulseOxGetSpO2();
uint8_t pulseOxGetRedLedCurrentBias();
void pulseOxSetOnBeatDetectedCallback(void (*cb)());
void pulseOxSetIRLedCurrent(LEDCurrent irLedNewCurrent);
void pulseOxiShutdown();
void pulseOxResume();

#endif /* MAX30100_PULSEOXIMETER_H_ */
