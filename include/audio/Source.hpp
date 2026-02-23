#ifndef AUDIO_SOURCE_HPP
#define AUDIO_SOURCE_HPP
#pragma once

#include "audio/Buffer.hpp"

namespace gpudsp::audio {

/*----------------------------------------------------------------------------*/

class Source final {
public:

    Source(void);
    ~Source(void);

    bool isValid(void);
    void play(void);
    void pause(void);
    void stop(void);

    void setLooping(const bool looping);
    void attachBuffer(const Buffer& buffer);

private:

    uint32_t m_al_source;

};

/*----------------------------------------------------------------------------*/

}

#endif