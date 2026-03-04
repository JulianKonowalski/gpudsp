#ifndef CORE_APP_HPP
#define CORE_APP_HPP
#pragma once

#include <cstdint>

#include "core/Audio.hpp"
#include "core/Event.hpp"
#include "core/Window.hpp"

#include "datastruct/RingBuffer.hpp"

namespace gpudsp::core {

/*----------------------------------------------------------------------------*/

class App final {
public:

    struct AppParameters {

        /**
         * This looks weird, but both clang and gcc 
         * will throw errors when using the default
         * constructor. It's a known bug.
         */
        inline static AppParameters Default(void) { return {}; }

        uint16_t screen_width = 400;
        uint16_t screen_height = 300;
        uint16_t audio_sample_rate = 44100;
    };

    static App* getInstance(const AppParameters& parameters = AppParameters::Default());

    inline const Audio& getAudio(void) { return *m_audio; }
    inline const Window& getWindow(void) { return *m_window; }

    void run(void);

private:

    App(const AppParameters& parameters);
    ~App(void);

    void audioCallback(float* buffer, uint16_t buffer_size, uint8_t num_channels);
    void eventCallback(Event& event);

    static App* s_instance;
    
    Audio* m_audio;
    Window* m_window;
    gpudsp::datastruct::RingBuffer<float, 2048, 4> m_audio_buffer;

};

/*----------------------------------------------------------------------------*/

}

#endif