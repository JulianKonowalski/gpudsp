#include <array>
#include <cstring>
#include <iostream>

#include <GLAD/glad.h>

#include "core/Audio.hpp"
#include "core/Window.hpp"

#include "gl/Shader.hpp"
#include "gl/Program.hpp"

const uint8_t NUM_CHANNELS = 2;
const uint16_t BUFFER_SIZE = 2048;
const uint16_t SAMPLE_RATE = 44100;
const uint32_t NUM_SAMPLES = BUFFER_SIZE * NUM_CHANNELS;

const char* shader_source = R"(
#version 430
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer DataBuffer {
    float data[];
};
void main() {
    uint idx = gl_GlobalInvocationID.x;
    data[2 * idx] = data[(2 * idx) + 1] = sin(idx * 2.0 * 3.1415 * 440.0 / 44100.0);
}
)";

int main(void) {
    gpudsp::core::Window* window = gpudsp::core::Window::getInstance();
    window->makeCurrent();

    gpudsp::gl::Program program;
    program.attachShader(gpudsp::gl::Shader(
        shader_source,
        gpudsp::gl::Shader::Type::COMPUTE
    ));
    program.link();

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        NUM_SAMPLES * sizeof(float),
        nullptr,
        GL_DYNAMIC_COPY
    );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    uint8_t read_idx = 0, write_idx = 0;
    std::array<std::array<float, NUM_SAMPLES>, 4> ring_buffer;
    gpudsp::core::Audio* audio = gpudsp::core::Audio::getInstance({
        [&read_idx, &write_idx, &ring_buffer](float* buffer, uint16_t buffer_size, uint8_t num_channels) {
            if (read_idx == write_idx) { return; }
            std::memcpy(buffer, ring_buffer[read_idx].data(), NUM_SAMPLES * sizeof(float));
            read_idx = (read_idx + 1) & 0b11;
        },
        SAMPLE_RATE,
        BUFFER_SIZE,
        true
    });

    while(!window->shouldClose()) { 
        window->update();
        if (((write_idx + 1) & 0b11) != read_idx) {
            program.bind();

            glDispatchCompute((NUM_SAMPLES / 256) / 2, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
            std::memcpy(ring_buffer[write_idx].data(), data, NUM_SAMPLES * sizeof(float));
            write_idx = (write_idx + 1) & 0b11;

            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            gpudsp::gl::Program::unbindAll();
        }
    }
}