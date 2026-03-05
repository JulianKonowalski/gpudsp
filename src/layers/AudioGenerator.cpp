#include "layers/AudioGenerator.hpp"

#include <cmath>
#include <string>

#include <GLAD/glad.h>

#include "core/App.hpp"

using namespace gpudsp::layers;

static const std::string s_shader_path(SHADER_PATH); 

/*----------------------------------------------------------------------------*/

AudioGenerator::AudioGenerator(
    void
) : m_audio_buffer(&gpudsp::core::App::getAudioBuffer()),
    m_gl_ssbo(0)
{
    const gpudsp::core::Audio& audio = gpudsp::core::App::getAudio();
    m_kernel_size = audio.getBufferSize() / 256;

    m_gl_program.attachShader(gpudsp::gl::Shader(
        s_shader_path + std::string("sine.glsl"),
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

AudioGenerator::~AudioGenerator(void) {}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onUpdate(void) {
    if (!m_audio_buffer->writeable()) { return; }

    m_gl_program.bind();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_gl_ssbo);

    glDispatchCompute(m_kernel_size, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_gl_ssbo);
    float* chunk = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    m_audio_buffer->writeChunk(chunk);

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    gpudsp::gl::Program::unbindAll();
}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onRender(void) {}

/*----------------------------------------------------------------------------*/

void AudioGenerator::onEvent(gpudsp::core::Event* event) {}

/*----------------------------------------------------------------------------*/