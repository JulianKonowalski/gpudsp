#include "gl/Shader.hpp"

#include <fstream>
#include <stdexcept>

#include <GLAD/glad.h>

using namespace gpudsp::gl;

/*----------------------------------------------------------------------------*/

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    std::string content(file_size, ' ');
    file.seekg(0);
    file.read(&content[0], file_size);
    return content;
}

/*----------------------------------------------------------------------------*/

Shader::Shader(
    const char* source,
    const Shader::Type type
) : m_gl_shader(0)
{
    switch(type) {
        case Type::COMPUTE:         m_gl_shader = glCreateShader(GL_COMPUTE_SHADER);            break;
        case Type::VERTEX:          m_gl_shader = glCreateShader(GL_VERTEX_SHADER);             break;
        case Type::TESS_CONTROL:    m_gl_shader = glCreateShader(GL_TESS_CONTROL_SHADER);       break;
        case Type::TESS_EVALUATION: m_gl_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);    break;
        case Type::GEOMETRY:        m_gl_shader = glCreateShader(GL_GEOMETRY_SHADER);           break;
        case Type::FRAGMENT:        m_gl_shader = glCreateShader(GL_FRAGMENT_SHADER);           break;
    }
    if (!m_gl_shader) { throw std::runtime_error("Failed to create a GL shader object"); }

    GLint success;
    glShaderSource(m_gl_shader, 1, &source, nullptr);
    glCompileShader(m_gl_shader);
    glGetShaderiv(m_gl_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(m_gl_shader, 512, nullptr, log);
        throw std::runtime_error(log);
    }
}

/*----------------------------------------------------------------------------*/

Shader::Shader(
    const std::string& filepath,
    const Shader::Type type
) : Shader(readFile(filepath).c_str(), type) {}

/*----------------------------------------------------------------------------*/

Shader::~Shader(void) { glDeleteShader(m_gl_shader); }

/*----------------------------------------------------------------------------*/