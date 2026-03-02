#include <vector>
#include <iostream>

#include <GLAD/glad.h>

#include "core/Window.hpp"

const char* shader_source = R"(
#version 430
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer DataBuffer {
    float data[];
};
void main() {
    uint idx = gl_GlobalInvocationID.x;
    data[idx] += 1.0;
}
)";

int main(void) {
    gpudsp::core::Window* window = gpudsp::core::Window::getInstance();
    window->makeCurrent();

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << "Shader compile error:\n" << log << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glDeleteShader(shader);

    const uint16_t count = 1024;
    std::vector<float> data(count);

    for (int i = 0; i < count; ++i) { data[i] = float(i); }

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        count * sizeof(float),
        data.data(),
        GL_DYNAMIC_COPY
    );

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    while(!window->shouldClose()) { 
        window->update();
        glUseProgram(program);
        glDispatchCompute(4, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        float* data = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

        for (int i = 0; i < count; ++i) { std::cout << data[i] << "\n"; }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        glUseProgram(0);
    }
}