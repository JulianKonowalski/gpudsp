#ifndef CORE_AUDIO_HPP
#define CORE_AUDIO_HPP
#pragma once

#include <thread>
#include <vector>
#include <cstdint>
#include <functional>

#include "al/Buffer.hpp"
#include "al/Device.hpp"
#include "al/Context.hpp"

namespace gpudsp::core {

/*----------------------------------------------------------------------------*/

class Audio final {
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
        bool is_running = false;
    };

    static Audio* getInstance(const AudioParameters& parameters = AudioParameters::Default());
    inline uint8_t getNumChannels(void) const { return 2; } // for now only 2 are supported
    inline uint16_t getSampleRate(void) const { return m_parameters.sample_rate; }
    inline uint16_t getBufferSize(void) const { return m_parameters.buffer_size; }
    inline bool isRunning(void) const { return m_parameters.is_running; }

    inline void setCallback(const Callback& callback) { m_parameters.callback = callback; }

    void start(void);
    void stop(void);

private:

    Audio(const AudioParameters& parameters);
    ~Audio(void);

    void run(void);
    void fillBuffer(gpudsp::al::Buffer& buffer);

    static Audio* s_instance;
    
    std::thread m_audio_thread;
    std::vector<float> m_samplesf;
    std::vector<int16_t> m_samplesi;

    gpudsp::al::Device m_device;
    gpudsp::al::Context m_context;

    AudioParameters m_parameters;

};

/*----------------------------------------------------------------------------*/

}

#endif