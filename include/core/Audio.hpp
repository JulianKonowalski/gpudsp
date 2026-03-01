#ifndef CORE_AUDIO_HPP
#define CORE_AUDIO_HPP
#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <cstdint>
#include <functional>

#include "audio/Buffer.hpp"
#include "audio/Device.hpp"
#include "audio/Context.hpp"

namespace gpudsp::core {

/*----------------------------------------------------------------------------*/

class Audio {
public:

    using Callback = std::function<void(float*,uint16_t,uint8_t)>;
    static const Callback s_default_callback;

    struct AudioParameters {

        /**
         * This looks weird, but both clang and gcc 
         * will throw errors when using the default
         * constructor. It's a known bug.
         */
        inline static AudioParameters Default(void) { return {}; }

        Callback callback = s_default_callback;
        uint16_t sample_rate = 0; //system's default
        uint16_t buffer_size = 2048;
    };

    static Audio* getInstance(const AudioParameters& parameters = AudioParameters::Default());

    inline void setCallback(const Callback& callback) { m_callback = callback; }

    void start(void);

private:

    Audio(const AudioParameters& parameters);
    ~Audio(void);

    void run(void);
    void fillBuffer(gpudsp::audio::Buffer& buffer);

    static Audio* s_instance;

    bool m_is_running;
    const uint16_t m_sample_rate;
    const uint16_t m_buffer_size;
    
    std::mutex m_mutex;
    std::thread m_audio_thread;
    std::vector<float> m_samplesf;
    std::vector<int16_t> m_samplesi;

    Callback m_callback;
    gpudsp::audio::Device m_device;
    gpudsp::audio::Context m_context;

};

/*----------------------------------------------------------------------------*/

}

#endif