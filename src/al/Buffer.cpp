#include "al/Buffer.hpp"

#include <stdexcept>

#include <AL/al.h>

using namespace gpudsp::al;

/*----------------------------------------------------------------------------*/

Buffer::Buffer(void) : m_al_buffer(0) {
    alGetError();
    alGenBuffers(1, &m_al_buffer);

#ifndef GPUDSP_AL_NOTHROW
    if (!m_al_buffer) {
        switch (alGetError()) {
            case AL_INVALID_VALUE: throw std::runtime_error("The buffer array isn't large enough to hold the number of buffers requested.");
            case AL_OUT_OF_MEMORY: throw std::runtime_error("There is not enough memory available to generate all the buffers requested.");
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/

Buffer::~Buffer(void) {
    alDeleteBuffers(1, &m_al_buffer);
    alGetError(); // clear any error codes
}

/*----------------------------------------------------------------------------*/

bool Buffer::isValid(void) { return (bool)alIsBuffer(m_al_buffer); }

/*----------------------------------------------------------------------------*/

uint32_t Buffer::getSize(void) { return this->getAlProperty(AL_SIZE); }

/*----------------------------------------------------------------------------*/

uint32_t Buffer::getBitDepth(void) { return this->getAlProperty(AL_BITS); }

/*----------------------------------------------------------------------------*/

uint32_t Buffer::getNumChannels(void) { return this->getAlProperty(AL_CHANNELS); }

/*----------------------------------------------------------------------------*/

uint32_t Buffer::getSampleRate(void) { return this->getAlProperty(AL_FREQUENCY); }

/*----------------------------------------------------------------------------*/

void Buffer::setData(
    const std::vector<uint8_t>& samples,
    const uint32_t sample_rate,
    const Type type
) {
    alBufferData(
        m_al_buffer,
        type == Type::MONO ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8,
        samples.data(),
        samples.size() * sizeof(uint8_t),
        sample_rate
    );
    
#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_OUT_OF_MEMORY:  throw std::runtime_error("There is not enough memory available to create this buffer.");
            case AL_INVALID_VALUE:  throw std::runtime_error("The size parameter is not valid for the format specified, the buffer is in use, or the data is a nullptr.");
            case AL_INVALID_ENUM:   throw std::runtime_error("The specified format does not exist.");
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/

void Buffer::setData(
    const std::vector<int16_t>& samples,
    const uint32_t sample_rate,
    const Type type
) {
    alBufferData(
        m_al_buffer,
        type == Type::MONO ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
        samples.data(),
        samples.size() * sizeof(int16_t),
        sample_rate
    );

#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch(err) {
            case AL_OUT_OF_MEMORY:  throw std::runtime_error("There is not enough memory available to create this buffer.");
            case AL_INVALID_VALUE:  throw std::runtime_error("The size parameter is not valid for the format specified, the buffer is in use, or the data is a nullptr.");
            case AL_INVALID_ENUM:   throw std::runtime_error("The specified format does not exist.");
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/

uint32_t Buffer::getAlProperty(const uint32_t property) {
    ALint value;
    alGetBufferi(m_al_buffer, property, &value);

#ifndef GPUDSP_AL_NOTHROW
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        switch (err) {
            case AL_INVALID_ENUM:   throw std::runtime_error("The specified parameter is not valid.");
            case AL_INVALID_NAME:   throw std::runtime_error("The specified buffer doesn't have parameters (the NULL buffer), or doesn't exist.");
            case AL_INVALID_VALUE:  throw std::runtime_error("The specified value pointer is not valid");
        }
    }
#endif

    return value;
}

/*----------------------------------------------------------------------------*/