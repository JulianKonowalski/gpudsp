#ifndef CORE_APP_HPP
#define CORE_APP_HPP
#pragma once

#include <queue>
#include <vector>
#include <cstdint>

#include "core/Layer.hpp"
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

        size_t audio_num_buffers = 4;
        size_t audio_buffer_size = 2048;
        uint16_t audio_sample_rate = 44100;
        uint16_t screen_width = 400;
        uint16_t screen_height = 300;
    };

    static App* getInstance(const AppParameters& parameters = AppParameters::Default());

    inline static bool exists(void) { return (bool)s_instance; }
    inline static const Audio& getAudio(void) { return *(s_instance->m_audio); }
    inline static const Window& getWindow(void) { return *(s_instance->m_window); }
    inline static gpudsp::datastruct::RingBuffer<float>& getAudioBuffer(void) { return s_instance->m_audio_buffer; }

    inline void pushLayer(Layer* layer) { m_layers.push_back(layer); }

    void run(void);

private:

    App(const AppParameters& parameters);
    ~App(void);

    void audioCallback(float* buffer, uint16_t buffer_size, uint8_t num_channels);
    void eventCallback(Event* event);

    static App* s_instance;
    
    Audio* m_audio;
    Window* m_window;
    gpudsp::datastruct::RingBuffer<float> m_audio_buffer;
    std::queue<Event*> m_event_queue;
    std::vector<Layer*> m_layers;

};

/*----------------------------------------------------------------------------*/

}

#endif