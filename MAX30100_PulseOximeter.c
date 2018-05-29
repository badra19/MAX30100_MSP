/*
 * MAX30100_PulseOximeter.c
 *
 *  Created on: 28 de mai de 2018
 *      Author: Davi Mendes
 */

#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"
#include "MAX30100_BeatDetector.h"
#include "MAX30100_BeatDetector.c"

#include "MAX30100_Filters.h"

#include "MAX30100_SpO2Calculator.h"
#include "MAX30100_SpO2Calculator.c"

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

    initWDT();
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
		beatDetected = beatDetectorAddSample(filteredPulseValue);

		if (beatDetectorGetRate() > 0) {
            state = PULSEOXIMETER_STATE_DETECTING;
            spO2CalcUpdate(irACValue, redACValue, beatDetected);
        } else if (state == PULSEOXIMETER_STATE_DETECTING) {
            state = PULSEOXIMETER_STATE_IDLE;
            spO2CalcReset();
        }

        switch (debuggingMode) {
            case PULSEOXIMETER_DEBUGGINGMODE_RAW_VALUES:
                /*
                Serial.print("R:");
                Serial.print(rawIRValue);
                Serial.print(",");
                Serial.println(rawRedValue);
                */
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES:
                /*
                Serial.print("R:");
                Serial.print(irACValue);
                Serial.print(",");
                Serial.println(redACValue);
                */
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT:
                /*
                Serial.print("R:");
                Serial.print(filteredPulseValue);
                Serial.print(",");
                Serial.println(beatDetectorGetCurrentThreshold());
                */
                break;

            default:
                break;
        }

        if (beatDetected && onBeatDetected) {
            onBeatDetected();
        }        
	}
}

void pulseOxCheckCurrentBias()
{
    // Follower that adjusts the red led current in order to have comparable DC baselines between
    // red and IR leds. The numbers are really magic: the less possible to avoid oscillations

    if (millis() - tsLastBiasCheck > CURRENT_ADJUSTMENT_PERIOD_MS) {
        bool changed = false;
        if (getDCW('I') - getDCW('R') > 70000 && redLedCurrentIndex < MAX30100_LED_CURR_50MA) {
            ++redLedCurrentIndex;
            changed = true;
        } else if (getDCW('R') - getDCW('I') > 70000 && redLedCurrentIndex > 0) {
            --redLedCurrentIndex;
            changed = true;
        }

        if (changed) {
            setLedsCurrent(irLedCurrent, (LEDCurrent)redLedCurrentIndex);
            tsLastCurrentAdjustment = millis();

            /*
            if (debuggingMode != PULSEOXIMETER_DEBUGGINGMODE_NONE) {
                Serial.print("I:");
                Serial.println(redLedCurrentIndex);
            }
            */
        }

        tsLastBiasCheck = millis();
    }

}

void pulseOxUpdate()
{
    update();

    pulseOxCheckSample();
    pulseOxCheckCurrentBias();
}

float pulseOxGetHeartRate()
{
    return beatDetectorGetRate();
}

uint8_t pulseOxGetSpO2()
{
    return spO2CalcGetSpO2();
}

uint8_t pulseOxGetRedLedCurrentBias()
{
    return redLedCurrentIndex;
}

void pulseOxSetOnBeatDetectedCallback(void (*cb)())
{
    onBeatDetected = cb;
}

void pulseOxSetIRLedCurrent(LEDCurrent irLedNewCurrent)
{
    irLedCurrent = irLedNewCurrent;
    setLedsCurrent(irLedCurrent, (LEDCurrent)redLedCurrentIndex);
}

void pulseOxiShutdown()
{
    shutdown();
}

void pulseOxResume()
{
    resume();
}
