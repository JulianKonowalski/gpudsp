#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP
#pragma once

#include <string>
#include <cstdint>

namespace gpudsp::gl {
 
/*----------------------------------------------------------------------------*/

class Shader {

    friend class Program;

public:

    enum class Type {
        COMPUTE,
        VERTEX,
        TESS_CONTROL,
        TESS_EVALUATION,
        GEOMETRY,
        FRAGMENT
    };

    Shader(const char* source, const Type type);
    Shader(const std::string& filepath, const Type type);
    ~Shader(void);

    Type getType(void);

private:

    uint32_t m_gl_shader;

};

/*----------------------------------------------------------------------------*/

}

#endif