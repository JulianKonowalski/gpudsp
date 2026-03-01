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

Audio* Audio::getInstance(
    const Callback& callback,
    const uint16_t sample_rate,
    const uint16_t buffer_size 
) {
    if (!s_instance) {
        if (!saladLoadALdefault()) { 
            throw std::runtime_error("Failed to initialize OpenAL functions"); 
        }       
        s_instance = new Audio(callback, sample_rate, buffer_size); 
    }
    return s_instance;
}

/*----------------------------------------------------------------------------*/

void Audio::start(void) {
    m_is_running = true;
    m_audio_thread = std::thread(&Audio::run, this);
    m_audio_thread.detach();
}

/*----------------------------------------------------------------------------*/

Audio::Audio(
    const Callback& callback,
    const uint16_t sample_rate,
    const uint16_t buffer_size
) : m_is_running(false),
    m_sample_rate(sample_rate),
    m_buffer_size(buffer_size),
    m_callback(callback),
    m_context(m_device, { sample_rate, 0, 0, 0, false }) 
{
    m_samplesf.resize(m_buffer_size * 2);
    m_samplesi.resize(m_buffer_size * 2);
}

/*----------------------------------------------------------------------------*/

Audio::~Audio(void) { 
    if (m_audio_thread.joinable()) { m_audio_thread.join(); }
    m_is_running = false;
    std::lock_guard<std::mutex> lock(m_mutex); // wait for Audio::run to finish
}

/*----------------------------------------------------------------------------*/

void Audio::run(void) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_context.makeCurrent();
    
    gpudsp::audio::Source source;
    gpudsp::audio::Buffer buffers[2];

    for (int i = 0; i < 2; ++i) { this->fillBuffer(buffers[i]); }
    source.queueBuffers({ buffers[0], buffers[1] });
    source.play();

    uint8_t buf_idx = 0;
    while (m_is_running) {
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
    m_callback(m_samplesf.data(), m_buffer_size, 2);
    for (int j = 0; j < m_buffer_size * 2; ++j) { m_samplesi[j] = m_samplesf[j] * 32767; }
    buffer.setData(m_samplesi, m_sample_rate, gpudsp::audio::Buffer::Type::STEREO);      
}

/*----------------------------------------------------------------------------*/