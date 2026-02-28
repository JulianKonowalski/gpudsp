#include "audio/Source.hpp"

#include <stdexcept>

#include <AL/al.h>

using namespace gpudsp::audio;

/*----------------------------------------------------------------------------*/

Source::Source(void) : m_al_source(0) {
    alGetError(); // clear any error codes
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
    #ifdef DEBUG
        bool is_valid = alIsSource(m_al_source);
        if (alGetError() != AL_NO_ERROR) {
            throw std::runtime_error("There is no current context.");
        }
        return is_valid;
    #else
        return (bool)alIsSource(m_al_source);
    #endif
}

/*----------------------------------------------------------------------------*/

void Source::play(void) {
    alSourcePlay(m_al_source);

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_INVALID_NAME:
                    throw std::runtime_error("The specified source name is not \
                        valid");
                break;
                case AL_INVALID_OPERATION:
                    throw std::runtime_error("The is no current context.");
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/

void Source::pause(void) {
    alSourcePause(m_al_source);

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_INVALID_NAME:
                    throw std::runtime_error("The specified source name is not \
                        valid");
                break;
                case AL_INVALID_OPERATION:
                    throw std::runtime_error("The is no current context.");
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/

void Source::stop(void) {
    alSourceStop(m_al_source);

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_INVALID_NAME:
                    throw std::runtime_error("The specified source name is not \
                        valid");
                break;
                case AL_INVALID_OPERATION:
                    throw std::runtime_error("The is no current context.");
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/

void Source::setLooping(const bool looping) {
    alSourcei(m_al_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_INVALID_VALUE:
                    throw std::runtime_error("The value given is out of range.");
                break;
                case AL_INVALID_ENUM:
                    throw std::runtime_error("The specified parameter is not \
                        valid");
                break;
                case AL_INVALID_NAME:
                    throw std::runtime_error("The specified source  name is \
                        not valid.");
                break;
                case AL_INVALID_OPERATION:
                    throw std::runtime_error("There is no current context.");
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/

void Source::attachBuffer(const Buffer& buffer) {
    alSourcei(m_al_source, AL_BUFFER, buffer.m_al_buffer);

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_INVALID_VALUE:
                    throw std::runtime_error("The value given is out of range.");
                break;
                case AL_INVALID_ENUM:
                    throw std::runtime_error("The specified parameter is not \
                        valid");
                break;
                case AL_INVALID_NAME:
                    throw std::runtime_error("The specified source  name is \
                        not valid.");
                break;
                case AL_INVALID_OPERATION:
                    throw std::runtime_error("There is no current context.");
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/