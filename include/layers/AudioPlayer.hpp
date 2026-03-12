#ifndef LAYERS_AUDIOPLAYER_HPP
#define LAYERS_AUDIOPLAYER_HPP
#pragma once

#include "core/Layer.hpp"

#include "gl/Program.hpp"

namespace gpudsp::layers {

/*----------------------------------------------------------------------------*/

class AudioPlayer : public gpudsp::core::Layer {
public:

    AudioPlayer(const float* const samples, const size_t num_samples);
    ~AudioPlayer(void) override;
    void onUpdate(void) override;
    void onRender(void) override;
    void onEvent(gpudsp::core::Event* event) override;

    inline void stop(void) { m_is_playing = false; m_playhead = 0; }
    inline void start(void) { m_is_playing = true; }
    inline void pause(void) { m_is_playing = false; }

private:

    gpudsp::gl::Program m_gl_program;

    size_t m_playhead;
    size_t m_num_samples;

    uint32_t m_gl_ssbo;
    uint32_t m_kernel_size;
    
    const float* m_samples;
    bool m_is_playing;

};

/*----------------------------------------------------------------------------*/

}

#endif