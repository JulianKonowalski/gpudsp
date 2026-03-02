#include "al/Buffer.hpp"

#include <stdexcept>

#include <AL/al.h>

using namespace gpudsp::al;

/*----------------------------------------------------------------------------*/

Buffer::Buffer(void) : m_al_buffer(0) {
    alGetError();
    alGenBuffers(1, &m_al_buffer);
    if (!m_al_buffer) {
        switch (alGetError()) {
            case AL_INVALID_VALUE:
                throw std::runtime_error(
                    "The buffer array isn't large enough to hold the number of "
                    "buffers requested."
                );
            break;
            case AL_OUT_OF_MEMORY:
                throw std::runtime_error(
                    "There is not enough memory available to generate all the "
                    "buffers requested."
                );
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/

Buffer::~Buffer(void) {
    alDeleteBuffers(1, &m_al_buffer);
    alGetError(); // clear any error codes
}

/*----------------------------------------------------------------------------*/

bool Buffer::isValid(void) { return (bool)alIsBuffer(m_al_buffer); }

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
    
    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_OUT_OF_MEMORY:
                    throw std::runtime_error(
                        "There is not enough memory available to create this "
                        "buffer."
                    );
                break;
                case AL_INVALID_VALUE:
                    throw std::runtime_error(
                        "The size parameter is not valid for the format "
                        "specified, the buffer is in use, or the data is a "
                        "nullptr."
                    );
                break;
                case AL_INVALID_ENUM:
                    throw std::runtime_error(
                        "The specified format does not exist."
                    );
                break;
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

    #ifdef DEBUG
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            switch(err) {
                case AL_OUT_OF_MEMORY:
                    throw std::runtime_error(
                        "There is not enough memory available to create this "
                        "buffer."
                    );
                break;
                case AL_INVALID_VALUE:
                    throw std::runtime_error(
                        "The size parameter is not valid for the format "
                        "specified, the buffer is in use, or the data is a "
                        "nullptr."
                    );
                break;
                case AL_INVALID_ENUM:
                    throw std::runtime_error(
                        "The specified format does not exist."
                    );
                break;
            }
        }
    #endif
}

/*----------------------------------------------------------------------------*/