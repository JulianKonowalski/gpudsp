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

    static Audio* getInstance(
        const Callback& callback = s_default_callback,
        const uint16_t sample_rate = 0, // system's default
        const uint16_t buffer_size = 2048
    );

    inline void setCallback(const Callback& callback) { m_callback = callback; }

    void start(void);

private:

    Audio(const Callback& callback, const uint16_t sample_rate, const uint16_t buffer_size);
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