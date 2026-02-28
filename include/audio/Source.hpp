#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP
#pragma once

#include "audio/Buffer.hpp"

namespace gpudsp::audio {

/*----------------------------------------------------------------------------*/

class Source final {
public:

    enum class State {
        PLAYING,
        PAUSED,
        STOPPED,
        UNDEFINED
    };

    Source(void);
    ~Source(void);

    void play(void);
    void pause(void);
    void stop(void);

    void attachBuffer(const Buffer& buffer);

    bool isValid(void);
    State getState(void);
    uint32_t getCurrentBuffer(void);
    uint32_t getNumQueuedBuffers(void);
    uint32_t getNumProcessedBuffers(void);

private:

    uint32_t getAlProperty(const uint32_t property);

    uint32_t m_al_source;

};

/*----------------------------------------------------------------------------*/

}

#endif