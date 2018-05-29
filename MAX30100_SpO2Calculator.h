/*
 * MAX30100_SpO2Calculator.h
 *
 *  Created on: 29 de mai de 2018
 *      Author: Davi Mendes
 */

#ifndef MAX30100_SPO2CALCULATOR_H_
#define MAX30100_SPO2CALCULATOR_H_

#include <stdint.h>

#define CALCULATE_EVERY_N_BEATS         3

void spO2CalcUpdate(float irACValue, float redACValue, bool beatDetected);
void spO2CalcReset();
uint8_t spO2CalcGetSpO2();

static const uint8_t spO2LUT[43];

float irACValueSqSum;
float redACValueSqSum;
uint8_t beatsDetectedNum;
uint32_t samplesRecorded;
uint8_t spO2;

#endif /* MAX30100_SPO2CALCULATOR_H_ */
