#ifndef GL_PROGRAM_HPP
#define GL_PROGRAM_HPP
#pragma once

#include <cstdint>

#include "gl/Shader.hpp "

namespace gpudsp::gl {

class Program {
public:

    Program(void);
    ~Program(void);

    void attachShader(const Shader& shader);
    void link(void);
    void bind(void);
    static void unbindAll(void);

private:

    uint32_t m_gl_program;

};

}

#endif