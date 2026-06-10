#ifndef AUDIO_H
#define AUDIO_H
#pragma once

#include <stdint.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------*/

typedef enum AudioBufferFormatType {
    MONO = 0,
    STEREO
} AudioBufferFormatType;

typedef struct AudioBuffer {
    int16_t*                samples;
    uint16_t                num_samples;   
    uint16_t                sample_rate;
    AudioBufferFormatType   type;
} AudioBuffer;

/*----------------------------------------------------------------------------*/

bool audio_initialize(void);
void audio_terminate(void);

void audio_queue_buffer(const AudioBuffer* p_buffer);
bool audio_is_buffer_queueable(void);
void audio_unqueue_processed_buffers(void);

void audio_start(void);
void audio_pause(void);
void audio_stop(void);

/*----------------------------------------------------------------------------*/

#endif