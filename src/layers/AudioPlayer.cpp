#include "layers/AudioPlayer.hpp"

#include <string>

#include <GLAD/glad.h>

#include "core/App.hpp"

using namespace gpudsp::layers;

static const std::string s_shader_path(SHADER_PATH); 

/*----------------------------------------------------------------------------*/

AudioPlayer::AudioPlayer(
    const float* samples,
    const size_t num_samples 
) : m_playhead(0),
    m_num_samples(num_samples),
    m_samples(samples),
    m_is_playing(true),
    m_gl_ssbo(0),
    m_kernel_size(0)
{
    const gpudsp::core::Audio& audio = gpudsp::core::App::getAudio();
    m_kernel_size = audio.getBufferSize() / 256;

    m_gl_program.attachShader(gpudsp::gl::Shader(
        s_shader_path + std::string("gain.glsl"),
        gpudsp::gl::Shader::Type::COMPUTE
    ));
    m_gl_program.link();

    glGenBuffers(1, &m_gl_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gl_ssbo);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        audio.getBufferSize() * audio.getNumChannels() * sizeof(float),
        nullptr,
        GL_DYNAMIC_COPY
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/*----------------------------------------------------------------------------*/

AudioPlayer::~AudioPlayer(void) {
    this->stop();
}

/*----------------------------------------------------------------------------*/

void AudioPlayer::onUpdate(void) {

    // TODO: clean this mess up

    if (!m_is_playing) { return; }

    gpudsp::datastruct::RingBuffer<float>& buffer = gpudsp::core::App::getAudioBuffer();
    if (!buffer.writeable()) { return; }
    size_t buffer_size = buffer.getChunkSize() / 2; // buffer size PER CHANNEL

    m_gl_program.bind();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gl_ssbo);

    if ((m_num_samples - m_playhead) < buffer_size) {
        buffer.writeChunkPadded(&m_samples[m_playhead * 2], (m_num_samples - m_playhead) * 2);
        m_playhead = 0; // loop to the beginning
        return;
    } else {
        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            buffer.getChunkSize() * sizeof(float),
            &m_samples[m_playhead * 2],
            GL_DYNAMIC_COPY
        );
        m_playhead += buffer_size;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_gl_ssbo);
    glDispatchCompute(m_kernel_size, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    float* chunk = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    buffer.writeChunk(chunk);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    gpudsp::gl::Program::unbindAll();
}

/*----------------------------------------------------------------------------*/

void AudioPlayer::onRender(void) {

}

/*----------------------------------------------------------------------------*/

void AudioPlayer::onEvent(gpudsp::core::Event* event) {

}

/*----------------------------------------------------------------------------*/