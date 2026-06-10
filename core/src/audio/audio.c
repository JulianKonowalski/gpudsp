#include "audio/audio.h"

#include <stdlib.h>

#include <AL/al.h>
#include <AL/alc.h>

/*----------------------------------------------------------------------------*/

typedef struct RingBuffer {
    ALuint* al_buffers;
    uint8_t read_ptr;
    uint8_t write_ptr;
    uint8_t size;
} RingBuffer;

void ring_buffer_initialize(RingBuffer* ring_buffer, uint8_t size) {
    ring_buffer->al_buffers = malloc(sizeof(ALuint) * size);
    ring_buffer->read_ptr   = 0;
    ring_buffer->write_ptr  = 0;
    ring_buffer->size       = size;
    alGenBuffers(size, ring_buffer->al_buffers);
}

void ring_buffer_terminate(RingBuffer* ring_buffer) {
    alDeleteBuffers(ring_buffer->size, ring_buffer->al_buffers);
    free(ring_buffer->al_buffers);
    ring_buffer->read_ptr   = 0;
    ring_buffer->write_ptr  = 0;
    ring_buffer->size       = 0;
}

bool ring_buffer_is_writable(const RingBuffer* ring_buffer) {
    return  (ring_buffer->write_ptr + 1) % ring_buffer->size != (ring_buffer->read_ptr);
}

bool ring_buffer_is_readable(const RingBuffer* ring_buffer) {
    return ring_buffer->read_ptr != ring_buffer->write_ptr;
}

/*----------------------------------------------------------------------------*/

static bool s_is_initialized = false;

static ALCdevice* s_alc_device     = (void*)0;
static ALCcontext* s_alc_context   = (void*)0;

static ALuint s_al_source          = 0;

static RingBuffer s_ring_buffer;

/*----------------------------------------------------------------------------*/

bool audio_initialize(void) {
    if (s_is_initialized) { return true; }

    s_alc_device = alcOpenDevice((void*)0);
    s_alc_context = alcCreateContext(s_alc_device, (void*)0);

    alcMakeContextCurrent(s_alc_context);
    alGenSources(1, &s_al_source);
    alSourcei(s_al_source, AL_LOOPING, AL_FALSE);

    ring_buffer_initialize(&s_ring_buffer, 16);
    
    s_is_initialized = true;

    return true;
}

/*----------------------------------------------------------------------------*/

void audio_terminate(void) {
    ring_buffer_terminate(&s_ring_buffer);
    alDeleteSources(1, &s_al_source);
    alcMakeContextCurrent((void*)0);
    alcDestroyContext(s_alc_context);
    alcCloseDevice(s_alc_device);
}

/*----------------------------------------------------------------------------*/

void audio_queue_buffer(const AudioBuffer* p_buffer) {
    alBufferData(
        s_ring_buffer.al_buffers[s_ring_buffer.write_ptr],
        p_buffer->type == MONO ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        p_buffer->samples,
        p_buffer->num_samples * sizeof(int16_t),
        p_buffer->sample_rate
    );
    alSourceQueueBuffers(s_al_source, 1, &s_ring_buffer.al_buffers[s_ring_buffer.write_ptr]);
    s_ring_buffer.write_ptr = (s_ring_buffer.write_ptr + 1) % s_ring_buffer.size;
}

/*----------------------------------------------------------------------------*/

bool audio_is_buffer_queueable(void) {
    return ring_buffer_is_writable(&s_ring_buffer);
}

/*----------------------------------------------------------------------------*/

void audio_unqueue_processed_buffers(void) {
    ALint num_processed_buffers = 0;
    alGetSourcei(s_al_source, AL_BUFFERS_PROCESSED, &num_processed_buffers);
    if (num_processed_buffers <= 0) { return; }

    ALuint tmp;
    for (int i = 0; i < num_processed_buffers; ++i) { alSourceUnqueueBuffers(s_al_source, 1, &tmp); }
    s_ring_buffer.read_ptr = (s_ring_buffer.read_ptr + num_processed_buffers) % s_ring_buffer.size;
}

/*----------------------------------------------------------------------------*/

void audio_start(void) { alSourcePlay(s_al_source); }
void audio_pause(void) { alSourcePause(s_al_source); }
void audio_stop(void) { alSourceStop(s_al_source); }

/*----------------------------------------------------------------------------*/