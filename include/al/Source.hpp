#ifndef AL_SOURCE_HPP
#define AL_SOURCE_HPP
#pragma once

#include <vector>

#include "al/Buffer.hpp"

namespace gpudsp::al {

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
    void queueBuffers(const std::vector<Buffer>& buffers);
    uint32_t unqueueOne(void);

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