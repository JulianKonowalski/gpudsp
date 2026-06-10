#ifndef API_H
#define API_H 
#pragma once

#include <stdlib.h>
#include <stdint.h>

#include <emscripten/emscripten.h>

/*----------------------------------------------------------------------------*/

typedef void (*APIStateChangeCallback)(float);

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_initialize(void);
void EMSCRIPTEN_KEEPALIVE api_terminate(void);

float* EMSCRIPTEN_KEEPALIVE api_allocate_buffer(size_t buffer_length);

void EMSCRIPTEN_KEEPALIVE api_play(float* l_channel, float* r_channel, size_t buffer_length);

float EMSCRIPTEN_KEEPALIVE api_get_input_gain(void);
float EMSCRIPTEN_KEEPALIVE api_get_output_gain(void);
float EMSCRIPTEN_KEEPALIVE api_get_eq_band_gain(uint8_t eq_band_idx);
float EMSCRIPTEN_KEEPALIVE api_get_playhead_position(void);

void EMSCRIPTEN_KEEPALIVE api_set_input_gain(float gain);
void EMSCRIPTEN_KEEPALIVE api_set_output_gain(float gain);
void EMSCRIPTEN_KEEPALIVE api_set_eq_band_gain(float eq_band_gain, uint8_t eq_band_idx);
void EMSCRIPTEN_KEEPALIVE api_set_playhead_position(float position);

void EMSCRIPTEN_KEEPALIVE api_set_playhead_position_change_callback(APIStateChangeCallback cb);

/*----------------------------------------------------------------------------*/

#endif