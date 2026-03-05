#include "layers/AudioGenerator.hpp"

#include <cmath>

#include "core/App.hpp"


using namespace gpudsp::layers;

/*----------------------------------------------------------------------------*/

AudioGenerator::AudioGenerator(
    void
) : m_audio_buffer(&gpudsp::core::App::getAudioBuffer()),
    m_sample_rate(gpudsp::core::App::getAudio().getSampleRate()),
    m_buffer_size(gpudsp::core::App::getAudio().getBufferSize()),
    m_buffer(new float[m_buffer_size * 2])
{
    float angle = 0.0f;
    float offset = 2.0f * 3.1415f * 440.0f / (float)m_sample_rate;
    for (int i = 0; i < m_buffer_size; ++i) {
        m_buffer[2 * i] = m_buffer[(2 * i) + 1] = sinf(angle);
        angle += offset;
        angle -= angle < 2.0f * 3.1415f ? 0.0f : 2.0f * 3.1415f;
    }
}

/*----------------------------------------------------------------------------*/

AudioGenerator::~AudioGenerator(void) {}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onUpdate(void) {
    if (!m_audio_buffer->writeable()) { return; }
    m_audio_buffer->writeChunk(m_buffer);
}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onRender(void) {}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onEvent(gpudsp::core::Event* event) {}

/*----------------------------------------------------------------------------*/