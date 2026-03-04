#ifndef AL_BUFFER_HPP 
#define AL_BUFFER_HPP
#pragma once

#include <vector>
#include <cstdint>

namespace gpudsp::al {

/*----------------------------------------------------------------------------*/

class Buffer final {

    friend class Source;

public:

    enum class Type { MONO, STEREO };

    Buffer(void);
    ~Buffer(void);

    bool isValid(void);
    uint32_t getSize(void);
    uint32_t getBitDepth(void);
    uint32_t getNumChannels(void);
    uint32_t getSampleRate(void);

    void setData(const std::vector<uint8_t>& samples, const uint32_t sample_rate, const Type type);
    void setData(const std::vector<int16_t>& samples, const uint32_t sample_rate, const Type type);

private:

    uint32_t getAlProperty(const uint32_t property);

    uint32_t m_al_buffer;

};

/*----------------------------------------------------------------------------*/

}

#endif