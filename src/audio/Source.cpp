#include "audio/Source.hpp"

#include <stdexcept>

#include <AL/al.h>

using namespace gpudsp::audio;

/*----------------------------------------------------------------------------*/

Source::Source(void) : m_al_source(0) {
    alGetError();
    alGenSources(1, &m_al_source);
    if (!m_al_source) {
        switch(alGetError()) {
            case AL_OUT_OF_MEMORY:
                throw std::runtime_error(
                    "There is not enough memory to generate all the requested \
                    sources."
                );
            break;
            case AL_INVALID_VALUE:
                throw std::runtime_error(
                    "There are not enough non-memory resources to create all \
                    the requested sources, or the array pointer is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error(
                    "There is no context to create sources in."
                );
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/

Source::~Source(void) {
    alDeleteSources(1, &m_al_source);
    alGetError(); // clear any error codes
}

/*----------------------------------------------------------------------------*/

bool Source::isValid(void) {
    return (bool)alIsSource(m_al_source);
}



/*----------------------------------------------------------------------------*/

void Source::play(void) { alSourcePlay(m_al_source); }

/*----------------------------------------------------------------------------*/

void Source::pause(void) { alSourcePause(m_al_source); }

/*----------------------------------------------------------------------------*/

void Source::stop(void) { alSourceStop(m_al_source); }

/*----------------------------------------------------------------------------*/

void Source::setLooping(const bool looping) {
    alSourcei(m_al_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
}

/*----------------------------------------------------------------------------*/

void Source::attachBuffer(const Buffer& buffer) {
    alSourcei(m_al_source, AL_BUFFER, buffer.m_al_buffer);
}

/*----------------------------------------------------------------------------*/