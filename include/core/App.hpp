#ifndef CORE_APP_HPP
#define CORE_APP_HPP
#pragma once

#include <cstdint>

#include "core/Audio.hpp"
#include "core/Window.hpp"

namespace gpudsp::core {

/*----------------------------------------------------------------------------*/

class App final {
public:

    static App* getInstance(
        const Audio::AudioParameters& audio_parameters = Audio::AudioParameters::Default(),
        const Window::WindowParameters& window_parameters = Window::WindowParameters::Default()
    );

    inline const Audio& getAudio(void) { return *m_audio; }
    inline const Window& getWindow(void) { return *m_window; }

    void run(void);

private:

    App(const Audio::AudioParameters& audio_parameters, const Window::WindowParameters& window_parameters);
    ~App(void);

    void audioCallback(float* buffer, uint16_t buffer_size, uint8_t num_channels);

    static App* s_instance;
    
    Audio* m_audio;
    Window* m_window;

};

/*----------------------------------------------------------------------------*/

}

#endif