#ifndef LAYERS_AUDIOGENERATOR_HPP
#define LAYERS_AUDIOGENERATOR_HPP
#pragma once

#include "core/Layer.hpp"

#include "gl/Program.hpp"

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

    gpudsp::gl::Program m_gl_program;
    gpudsp::datastruct::RingBuffer<float>* m_audio_buffer;
    uint32_t m_gl_ssbo;
    uint32_t m_kernel_size;

};

/*----------------------------------------------------------------------------*/

}

#endif