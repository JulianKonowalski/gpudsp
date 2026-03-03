#include "gl/Program.hpp"

#include <stdexcept>

#include <GLAD/glad.h>

using namespace gpudsp::gl;

/*----------------------------------------------------------------------------*/

Program::Program(void) : m_gl_program(0) {
    m_gl_program = glCreateProgram();
    if (!m_gl_program) { throw std::runtime_error("Failed to create a GL program object"); }
}

/*----------------------------------------------------------------------------*/

Program::~Program(void) { glDeleteProgram(m_gl_program); }

/*----------------------------------------------------------------------------*/

void Program::attachShader(const Shader& shader) {
    glAttachShader(m_gl_program, shader.m_gl_shader);
}

/*----------------------------------------------------------------------------*/

void Program::link(void) { glLinkProgram(m_gl_program); }

/*----------------------------------------------------------------------------*/

void Program::bind(void) { glUseProgram(m_gl_program); }

/*----------------------------------------------------------------------------*/

void Program::unbindAll(void) { glUseProgram(0); }

/*----------------------------------------------------------------------------*/