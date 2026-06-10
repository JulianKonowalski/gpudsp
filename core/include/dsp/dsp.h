#ifndef DSP_H
#define DSP_H
#pragma once

#include "audio/audio.h"

/*----------------------------------------------------------------------------*/

bool dsp_initialize(unsigned int buffer_size, uint8_t num_channels);
void dsp_terminate(void);

void dsp_process(float* samples, uint16_t num_samples, AudioBufferFormatType type);

float dsp_get_input_gain(void);
float dsp_get_output_gain(void);
float dsp_get_eq_band_gain(uint8_t eq_band_idx);

void dsp_set_input_gain(float gain);
void dsp_set_output_gain(float gain);
void dsp_set_eq_band_gain(float eq_band_gain, uint8_t eq_band_idx);

/*----------------------------------------------------------------------------*/

#endif