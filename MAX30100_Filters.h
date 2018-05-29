/*
 * MAX30100_Filters.h
 *
 *  Created on: 28 de mai de 2018
 *      Author: Davi Mendes
 */

#ifndef MAX30100_FILTERS_H_
#define MAX30100_FILTERS_H_

// http://www.schwietering.com/jayduino/filtuino/
// Low pass butterworth filter order=1 alpha1=0.1
// Fs=100Hz, Fc=6Hz
float v[2];

float butterworthStep(float x) //class II
{
	v[0] = v[1];
	v[1] = (2.452372752527856026e-1 * x)
		 + (0.50952544949442879485 * v[0]);
	return
		 (v[0] + v[1]);
}	

// http://sam-koblenski.blogspot.de/2015/11/everyday-dsp-for-programmers-dc-and.html
float alpha;
float dcw = 0;

void setDcAlpha(float alpha_)
	alpha = alpha_;

float getDCW()
	return dcw;

float dcStep(float x)
{
	float olddcw = dcw;
	dcw = (float)x + alpha * dcw;

	return dcw - olddcw;
}

#endif /* MAX30100_FILTERS_H_ */
