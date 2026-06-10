#include "api.h"

#include <math.h>
#include <string.h>

#include <emscripten/html5.h>
#include <emscripten/wasm_worker.h>

#include "dsp/dsp.h"
#include "audio/audio.h"

/*----------------------------------------------------------------------------*/

static size_t s_playhead    = 0;
static size_t s_num_samples = 0;
static float* s_l_channel   = (void*)0;
static float* s_r_channel   = (void*)0;

static float* s_l_tmp_buffer = (void*)0;
static float* s_r_tmp_buffer = (void*)0;

static AudioBuffer s_audio_buffers[2];
static uint8_t s_audio_buffer_read_idx = 0;
static uint8_t s_audio_buffer_write_idx = 1;

/*----------------------------------------------------------------------------*/

void default_api_state_change_callback(float v) { }

APIStateChangeCallback s_on_playhead_position_change = default_api_state_change_callback;

/*----------------------------------------------------------------------------*/

bool animation_frame(double time, void* p_user_data) {
    audio_unqueue_processed_buffers();

    while (audio_is_buffer_queueable()) {
        if (!s_l_channel || !s_r_channel) {
            memset(s_audio_buffers[s_audio_buffer_write_idx].samples,
                0, sizeof(int16_t) * s_audio_buffers[s_audio_buffer_write_idx].num_samples);
        } else {

            int buffer_size         = s_audio_buffers[s_audio_buffer_write_idx].num_samples / 2;
            int buffer_half_size    = buffer_size / 2;

            memcpy(s_l_tmp_buffer, &s_l_channel[s_playhead], sizeof(float) * buffer_size);
            memcpy(s_r_tmp_buffer, &s_r_channel[s_playhead], sizeof(float) * buffer_size);

            dsp_process(s_l_tmp_buffer, buffer_size, MONO);
            dsp_process(s_r_tmp_buffer, buffer_size, MONO);

            for (int i = 0; i < buffer_size; ++i) {
                int buffer_sample_idx = i * 2;
                int channel_sample_idx = (s_playhead + i) % s_num_samples;

                s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx]        = floor(s_l_tmp_buffer[i] * 32768.0f);
                s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx + 1]    = floor(s_r_tmp_buffer[i] * 32768.0f);

                if (i < buffer_half_size) {
                    if (s_audio_buffer_read_idx == 0) {
                        s_audio_buffers[s_audio_buffer_read_idx].samples[buffer_sample_idx + (buffer_half_size * 2)] +=
                            s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx];
                        s_audio_buffers[s_audio_buffer_read_idx].samples[buffer_sample_idx + (buffer_half_size * 2) + 1] +=
                            s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx + 1];
                    } else {
                        s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx] +=
                            s_audio_buffers[s_audio_buffer_read_idx].samples[buffer_sample_idx + (buffer_half_size * 2)];
                        s_audio_buffers[s_audio_buffer_write_idx].samples[buffer_sample_idx + 1] +=
                            s_audio_buffers[s_audio_buffer_read_idx].samples[buffer_sample_idx + (buffer_half_size * 2)+ 1];                       
                    }
                }
            }
            s_playhead = (s_playhead + buffer_half_size - 1) % s_num_samples;
        }

        if (s_audio_buffer_read_idx == 0) { audio_queue_buffer(&s_audio_buffers[s_audio_buffer_read_idx]); }

        s_audio_buffer_read_idx     = s_audio_buffer_write_idx;
        s_audio_buffer_write_idx    = 1 - s_audio_buffer_write_idx;
    }
    s_num_samples == 0 ? s_on_playhead_position_change(0.0f)
        : s_on_playhead_position_change(s_playhead / (float)s_num_samples);
    return true;
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_initialize(void) {
    dsp_initialize(2048, 1);
    audio_initialize();

    for (int i = 0; i < 2; ++i) {
        s_audio_buffers[i].num_samples = 4096;
        s_audio_buffers[i].sample_rate = 44100;
        s_audio_buffers[i].type = STEREO;
        s_audio_buffers[i].samples = malloc(sizeof(int16_t) * s_audio_buffers[i].num_samples);
        memset(s_audio_buffers[i].samples, 0, sizeof(int16_t) * s_audio_buffers[i].num_samples);
    }

    // prefill audio buffer with silence
    while(audio_is_buffer_queueable()) { audio_queue_buffer(&s_audio_buffers[0]); }

    audio_start();
    emscripten_request_animation_frame_loop(animation_frame, (void*)0);
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_terminate(void) {
    audio_stop();
    audio_terminate();
    dsp_terminate();
    
    if (s_l_channel) { free(s_l_channel); }
    if (s_r_channel) { free(s_r_channel); }
    if (s_l_tmp_buffer) { free(s_l_tmp_buffer); }
    if (s_r_tmp_buffer) { free(s_r_tmp_buffer); }
}

/*----------------------------------------------------------------------------*/

float* EMSCRIPTEN_KEEPALIVE api_allocate_buffer(size_t buffer_length) {
    return malloc(sizeof(float) * buffer_length);
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_play(
    float* l_channel,
    float* r_channel,
    size_t buffer_length
) {
    if (s_l_channel) { free(s_l_channel); }
    if (s_r_channel) { free(s_r_channel); }

    s_l_channel     = l_channel;
    s_r_channel     = r_channel;

    s_l_tmp_buffer = (float*)malloc(sizeof(float) * s_audio_buffers[0].num_samples / 2);
    s_r_tmp_buffer = (float*)malloc(sizeof(float) * s_audio_buffers[0].num_samples / 2);

    s_num_samples   = buffer_length;
}

/*----------------------------------------------------------------------------*/

float EMSCRIPTEN_KEEPALIVE api_get_input_gain(void) { return dsp_get_input_gain(); }

/*----------------------------------------------------------------------------*/

float EMSCRIPTEN_KEEPALIVE api_get_output_gain(void) { return dsp_get_output_gain(); }

/*----------------------------------------------------------------------------*/

float EMSCRIPTEN_KEEPALIVE api_get_eq_band_gain(uint8_t eq_band_idx) {
    return dsp_get_eq_band_gain(eq_band_idx);
}

/*----------------------------------------------------------------------------*/

float EMSCRIPTEN_KEEPALIVE api_get_playhead_position(void) {
    return s_playhead / (float)s_num_samples;
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_set_input_gain(float gain) {
    dsp_set_input_gain(gain);
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_set_output_gain(float gain) {
    dsp_set_output_gain(gain);
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_set_eq_band_gain(
    float eq_band_gain,
    uint8_t eq_band_idx
) { dsp_set_eq_band_gain(eq_band_gain, eq_band_idx); }

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_set_playhead_position(float position) {
    s_playhead = (size_t)(position * s_num_samples);
    if (s_playhead < 0 || s_playhead >= s_num_samples) { s_playhead = 0; }
}

/*----------------------------------------------------------------------------*/

void EMSCRIPTEN_KEEPALIVE api_set_playhead_position_change_callback(
    APIStateChangeCallback cb
) { s_on_playhead_position_change = cb; }

/*----------------------------------------------------------------------------*/