#include "core/Audio.hpp"

#include <thread>
#include <cstring>
#include <stdexcept>

#include <AL/salad.h>

#include "audio/Source.hpp"

using namespace gpudsp::core;

/*----------------------------------------------------------------------------*/

Audio* Audio::s_instance = nullptr;

const Audio::Callback Audio::s_default_callback = [](float* b, uint16_t s, uint8_t c) { 
    std::memset(b, 0, s * c * sizeof(float)); 
};

/*----------------------------------------------------------------------------*/

Audio* Audio::getInstance(const AudioParameters& parameters) {
    if (!s_instance) { s_instance = new Audio(parameters); }
    return s_instance;
}

/*----------------------------------------------------------------------------*/

void Audio::start(void) {
    m_parameters.is_running = true;
    m_audio_thread = std::thread(&Audio::run, this);
    m_audio_thread.detach();
}

/*----------------------------------------------------------------------------*/

void Audio::stop(void) { 
    if (m_audio_thread.joinable()) { m_audio_thread.join(); }
    m_parameters.is_running = false; 
}

/*----------------------------------------------------------------------------*/

Audio::Audio(
    const AudioParameters& parameters
) : m_parameters(parameters),
    m_context(m_device, { parameters.sample_rate, 0, 0, 0, false }) 
{
    if (!saladLoadALdefault()) { 
        throw std::runtime_error("Failed to initialize OpenAL functions"); 
    }

    // both channels are stored in one, contiguous buffer
    uint32_t buffer_size = m_parameters.buffer_size * 2; 
    m_samplesf.resize(buffer_size);
    m_samplesi.resize(buffer_size);
    if (m_parameters.is_running) { this->start(); }
}

/*----------------------------------------------------------------------------*/

Audio::~Audio(void) { this->stop(); }

/*----------------------------------------------------------------------------*/

void Audio::run(void) {
    m_context.makeCurrent();
    
    gpudsp::audio::Source source;
    gpudsp::audio::Buffer buffers[2];

    for (int i = 0; i < 2; ++i) { this->fillBuffer(buffers[i]); }
    source.queueBuffers({ buffers[0], buffers[1] });
    source.play();

    uint8_t buf_idx = 0;
    while (m_parameters.is_running) {
        while (source.getNumProcessedBuffers() > 0) {
            source.unqueueOne();
            this->fillBuffer(buffers[buf_idx]);
            source.queueBuffers({ buffers[buf_idx] });
            buf_idx = (buf_idx + 1) & 0x1;
        }
    }

    source.stop();
}

/*----------------------------------------------------------------------------*/

void Audio::fillBuffer(gpudsp::audio::Buffer& buffer) {
    m_parameters.callback(m_samplesf.data(), m_parameters.buffer_size, 2);
    for (int j = 0; j < m_parameters.buffer_size * 2; ++j) { m_samplesi[j] = m_samplesf[j] * 32767; }
    buffer.setData(m_samplesi, m_parameters.sample_rate, gpudsp::audio::Buffer::Type::STEREO);      
}

/*----------------------------------------------------------------------------*/