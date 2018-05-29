/*
 * MAX30100_PulseOximeter.c
 *
 *  Created on: 28 de mai de 2018
 *      Author: Davi Mendes
 */

#include "MAX30100_PulseOximeter.h"

bool pulseOxBegin(PulseOximeterDebuggingMode debuggingMode_)
{
	debuggingMode = debuggingMode_;

	bool ready = begin();

	if(!ready)
	{
		if(debuggingMode != PULSEOXIMETER_DEBUGGINGMODE_NONE)
			sendString("Failed to initialize the HRM sensor");
		return false;
	}

	setMode(MAX30100_MODE_SPO2_HR);
    setLedsCurrent(irLedCurrent, (LEDCurrent)redLedCurrentIndex);

    setDCAlpha(DC_REMOVER_ALPHA, 'R');
    setDCAlpha(DC_REMOVER_ALPHA, 'I');

    state = PULSEOXIMETER_STATE_IDLE;

    return true;
}

void pulseOxCheckSample()
{
	uint16_t rawIRValue, rawRedValue;
	float irACValue, redACValue, filteredPulseValue;
	bool beatDetected; 

	// Dequeue all available samples
	while(getRawValues(&rawIRValue, &rawRedValue))
	{
        irACValue = dcStepIr(rawIRValue);
        redACValue = dcStepRed(rawRedValue);

        filteredPulseValue = butterworthStep(-irACValue);
	}
}
