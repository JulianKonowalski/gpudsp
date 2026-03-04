#include "al/Source.hpp"

#include <stdexcept>

#include <AL/al.h>

using namespace gpudsp::al;

/*----------------------------------------------------------------------------*/

Source::Source(void) : m_al_source(0) {
    alGetError(); // clear any error codes
    alGenSources(1, &m_al_source);

#ifndef GPUDSP_AL_NOTHROW
    if (!m_al_source) {
        switch(alGetError()) {
            case AL_OUT_OF_MEMORY:
                throw std::runtime_error(
                    "There is not enough memory to generate all the requested "
                    "sources."
                );
            break;
            case AL_INVALID_VALUE:
                throw std::runtime_error(
                    "There are not enough non-memory resources to create all "
                    "the requested sources, or the array pointer is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error(
                    "There is no context to create sources in."
                );
            break;
        }
    }
#endif

}

/*----------------------------------------------------------------------------*/

Source::~Source(void) {
    alDeleteSources(1, &m_al_source);
    alGetError(); // clear any error codes
}

/*----------------------------------------------------------------------------*/

void Source::play(void) {
    alSourcePlay(m_al_source);
#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source name is not valid."
                );
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

#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source name is not valid."
                );
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
#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source name is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error("The is no current context.");
            break;
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/

void Source::attachBuffer(const Buffer& buffer) {
    alSourcei(m_al_source, AL_BUFFER, buffer.m_al_buffer);

#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_VALUE:
                throw std::runtime_error("The value given is out of range.");
            break;
            case AL_INVALID_ENUM:
                throw std::runtime_error(
                    "The specified parameter is not valid."
                );
            break;
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source  name is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error("There is no current context.");
            break;
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/

void Source::queueBuffers(const std::vector<Buffer>& buffers) {
    std::vector<ALuint> buffer_ids(buffers.size());
    for (int i = 0; i < buffers.size(); ++i) { buffer_ids[i] = buffers[i].m_al_buffer; }
    alSourceQueueBuffers(m_al_source, buffer_ids.size(), buffer_ids.data());

#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "At least one specified buffer name is not valid, or "
                    "the specified source name is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error(
                    "There is no current context, an attempt was made to "
                    "add a new buffer which is not the same format as the "
                    "buffers already in the queue, or the source already "
                    "has a static buffer attached."
                );
            break;                
        }
    }   
#endif
}

/*----------------------------------------------------------------------------*/

uint32_t Source::unqueueOne(void) {
    uint32_t buffer_id;
    alSourceUnqueueBuffers(m_al_source, 1, &buffer_id);
    
#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_VALUE:
                throw std::runtime_error(
                    "At least one buffer can not be unqueued because it has "
                    "not been processed yet."
                );
            break;
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source name is not valid."
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error("The is no current context.");
            break;
        }
    }
#endif

    return buffer_id;
}

/*----------------------------------------------------------------------------*/

bool Source::isValid(void) {
#ifndef GPUDSP_AL_NOTHROW
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

Source::State Source::getState(void) {
    switch(getAlProperty(AL_SOURCE_STATE)) {
        case AL_PLAYING: return Source::State::PLAYING;
        case AL_PAUSED: return Source::State::PAUSED;
        case AL_STOPPED: return Source::State::STOPPED;
        default: return Source::State::UNDEFINED;
    }
}

/*----------------------------------------------------------------------------*/

uint32_t Source::getCurrentBuffer(void) { return this->getAlProperty(AL_BUFFER); }

/*----------------------------------------------------------------------------*/

uint32_t Source::getNumQueuedBuffers(void) { return this->getAlProperty(AL_BUFFERS_QUEUED); }

/*----------------------------------------------------------------------------*/

uint32_t Source::getNumProcessedBuffers(void) { return this->getAlProperty(AL_BUFFERS_PROCESSED); }

/*----------------------------------------------------------------------------*/

uint32_t Source::getAlProperty(const uint32_t property) {
    ALint value;
    alGetSourcei(m_al_source, property, &value);
    
#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_INVALID_VALUE:
                throw std::runtime_error(
                    "The value pointer given is not valid."
                );
            break;
            case AL_INVALID_ENUM:
                throw std::runtime_error(
                    "The specified parameter is not valid."
                );
            break;
            case AL_INVALID_NAME:
                throw std::runtime_error(
                    "The specified source name is not valid"
                );
            break;
            case AL_INVALID_OPERATION:
                throw std::runtime_error("There is no current context.");
            break;
        }
    }
#endif

    return value;
}

/*----------------------------------------------------------------------------*/