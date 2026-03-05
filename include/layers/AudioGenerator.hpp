#ifndef LAYERS_AUDIOGENERATOR_HPP
#define LAYERS_AUDIOGENERATOR_HPP
#pragma once

#include "core/Layer.hpp"

#include "datastruct/RingBuffer.hpp"

namespace gpudsp::layers {

/*----------------------------------------------------------------------------*/

class AudioGenerator : public gpudsp::core::Layer {
public:

    AudioGenerator(void);
    ~AudioGenerator(void) override;

    void onUpdate(void) override;
    void onRender(void) override;
    void onEvent(gpudsp::core::Event* event) override;

private:

    gpudsp::datastruct::RingBuffer<float>* m_audio_buffer;
    uint16_t m_sample_rate;
    uint16_t m_buffer_size;
    uint8_t m_num_channels;
    float* m_buffer;

};

/*----------------------------------------------------------------------------*/

}

#endif