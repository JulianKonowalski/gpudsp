#include "dsp/dsp.h"

#include <stdio.h>

#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>

#include <GLES3/gl3.h>

/*----------------------------------------------------------------------------*/

static EMSCRIPTEN_WEBGL_CONTEXT_HANDLE s_webglctx_handle = 0;

static GLuint s_program_eq          = 0;
static GLuint s_program_fft         = 0;
static GLuint s_program_hanning     = 0;
static GLuint s_program_bit_reverse = 0;

static GLuint s_texture_audio_buffer    = 0;
static GLuint s_texture_eq_curve_LUT    = 0;
static GLuint s_texture_bit_reverse_LUT = 0;
static GLuint s_texture_framebuffers[2] = {0, 0};

static GLuint s_framebuffers[2] = {0, 0};

static GLuint s_uniform_fft_stage           = 0;
static GLuint s_uniform_fft_inverse         = 0;
static GLuint s_uniform_fft_buffer_size     = 0;
static GLuint s_uniform_bit_reverse_gain    = 0;

static float s_input_gain   = 1.0f;
static float s_output_gain  = 0.5f;
static float s_eq_bands[]   = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

/*----------------------------------------------------------------------------*/

GLuint load_shader_from_file(const char* p_filepath, const GLenum shader_type) {
    FILE* p_file = fopen(p_filepath, "rb");
    if (!p_file) { return 0; }

    fseek(p_file, 0, SEEK_END);
    long file_size = ftell(p_file);
    fseek(p_file, 0, SEEK_SET);

    char* p_data;
    p_data = (char*)malloc(sizeof(char) * (file_size) + 1);
    fread((void*)p_data, sizeof(unsigned char), file_size, p_file);
    p_data[file_size] = '\0';

    fclose(p_file);

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const GLchar* const*)&p_data, (void*)0);
    glCompileShader(shader);
    free(p_data);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, (void*)0, info_log);
        printf("Vertex shader failed to compile:\n%s\n", info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint create_program(const GLuint v_shader, const GLuint f_shader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, (void*)0, info_log);
        printf("GL program failed to link:\n%s\n", info_log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

float* create_bit_reverse_LUT(uint16_t size) {
    float* LUT = (float*)malloc(sizeof(float) * size);
    for (int i = 0; i < size; ++i) {
        uint16_t x = i, y = 0;
        for (uint8_t j = 0; j < 16; ++j) {
            y = (y << 1) | (x & 1);
            x >>= 1;
        }
        LUT[i] = (float)y;
    }
    return LUT;
}

/*----------------------------------------------------------------------------*/

bool dsp_initialize(unsigned int buffer_size, uint8_t num_channels) {
    { // webgl context
        EmscriptenWebGLContextAttributes webgl_context_attributes;
        emscripten_webgl_init_context_attributes(&webgl_context_attributes);

        webgl_context_attributes.majorVersion               = 2;
        webgl_context_attributes.alpha                      = EM_FALSE;
        webgl_context_attributes.depth                      = EM_FALSE;
        webgl_context_attributes.antialias                  = EM_FALSE;
        webgl_context_attributes.premultipliedAlpha         = EM_FALSE;
        webgl_context_attributes.preserveDrawingBuffer      = EM_FALSE;
        webgl_context_attributes.enableExtensionsByDefault  = EM_FALSE;

        s_webglctx_handle = emscripten_webgl_create_context("#gl_canvas", &webgl_context_attributes);
        if (!s_webglctx_handle) { return false; }
        if (!emscripten_webgl_enable_extension(s_webglctx_handle, "EXT_color_buffer_float")) {
            printf("ERROR: Float render targets not supported\n");
            return false;
        }

        emscripten_webgl_make_context_current(s_webglctx_handle);
    } // !webgl context

    { // shaders
        GLuint shader_eq            = load_shader_from_file("shaders/eq.frag",          GL_FRAGMENT_SHADER);
        GLuint shader_fft           = load_shader_from_file("shaders/fft.frag",         GL_FRAGMENT_SHADER);
        GLuint shader_vertex        = load_shader_from_file("shaders/dsp.vert",         GL_VERTEX_SHADER);
        GLuint shader_hanning       = load_shader_from_file("shaders/hanning.frag",     GL_FRAGMENT_SHADER);
        GLuint shader_bit_reverse   = load_shader_from_file("shaders/bit_reverse.frag", GL_FRAGMENT_SHADER);
        if (!shader_eq || !shader_fft || !shader_vertex || !shader_hanning || !shader_bit_reverse) { return false; }

        s_program_eq            = create_program(shader_vertex, shader_eq);
        s_program_fft           = create_program(shader_vertex, shader_fft);
        s_program_hanning       = create_program(shader_vertex, shader_hanning);
        s_program_bit_reverse   = create_program(shader_vertex, shader_bit_reverse);
        if (!s_program_eq || !s_program_fft || !s_program_hanning || !s_program_bit_reverse) { return false; }

        glDeleteShader(shader_eq);
        glDeleteShader(shader_fft);
        glDeleteShader(shader_vertex);
        glDeleteShader(shader_hanning);
        glDeleteShader(shader_bit_reverse);
    } // !shaders

    { // textures 
        glGenTextures(1, &s_texture_eq_curve_LUT);
        glBindTexture(GL_TEXTURE_2D, s_texture_eq_curve_LUT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 10, 1, 0, GL_RED, GL_FLOAT, s_eq_bands);

        float* LUT = create_bit_reverse_LUT(buffer_size);
        glGenTextures(1, &s_texture_bit_reverse_LUT);
        glBindTexture(GL_TEXTURE_2D, s_texture_bit_reverse_LUT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, buffer_size, 1, 0, GL_RED, GL_FLOAT, LUT);
        free(LUT);

        glGenTextures(1, &s_texture_audio_buffer);
        glBindTexture(GL_TEXTURE_2D, s_texture_audio_buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);       
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, buffer_size, num_channels, 0, GL_RED, GL_FLOAT, (void*)0);
        
        glGenTextures(2, s_texture_framebuffers);
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, buffer_size, num_channels, 0, GL_RG, GL_FLOAT, (void*)0);           
        }

        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    } // !textures

    { // framebuffers
        glGenFramebuffers(2, s_framebuffers);
        for (int i = 0; i < 2; ++i) {
            glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_texture_framebuffers[i], 0);
        }
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } // !framebuffers

    { // uniforms
        glUseProgram(s_program_eq);
        glUniform1i(glGetUniformLocation(s_program_eq, "u_buffer_input"), 0);
        glUniform1i(glGetUniformLocation(s_program_eq, "u_lut_eq_curve"), 1);

        glUseProgram(s_program_fft);
        s_uniform_fft_stage         = glGetUniformLocation(s_program_fft, "u_fft_stage");
        s_uniform_fft_inverse       = glGetUniformLocation(s_program_fft, "u_fft_inverse");
        s_uniform_fft_buffer_size   = glGetUniformLocation(s_program_fft, "u_fft_buffer_size");
        glUniform1i(glGetUniformLocation(s_program_fft, "u_buffer_input"), 0);

        glUseProgram(s_program_hanning);
        glUniform1i(glGetUniformLocation(s_program_hanning, "u_buffer_input"), 0);

        glUseProgram(s_program_bit_reverse);
        s_uniform_bit_reverse_gain = glGetUniformLocation(s_program_bit_reverse, "u_gain");
        glUniform1f(s_uniform_bit_reverse_gain, s_input_gain);
        glUniform1i(glGetUniformLocation(s_program_bit_reverse, "u_buffer_input"), 0);
        glUniform1i(glGetUniformLocation(s_program_bit_reverse, "u_lut_bit_reverse"), 1);

        glUseProgram(GL_NONE);
    } // !uniforms
    
    return true;
}

/*----------------------------------------------------------------------------*/

void dsp_terminate(void) {
    glDeleteTextures(1, &s_texture_audio_buffer);
    glDeleteTextures(1, &s_texture_bit_reverse_LUT);
    glDeleteTextures(2, s_texture_framebuffers);
    glDeleteFramebuffers(2, s_framebuffers);

    glDeleteProgram(s_program_eq);
    glDeleteProgram(s_program_fft);
    glDeleteProgram(s_program_bit_reverse);

    emscripten_webgl_make_context_current(0);
    emscripten_webgl_destroy_context(s_webglctx_handle);   
}

/*----------------------------------------------------------------------------*/

void dsp_process(
    float* samples,
    uint16_t num_samples,
    AudioBufferFormatType type
) {
    uint8_t num_channels = (type == MONO) ? 1 : 2;
    uint8_t write_idx = 0, read_idx = 1;

    glViewport(0, 0, num_samples, num_channels);

    { // update audio
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_texture_audio_buffer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, num_samples, num_channels, GL_RED, GL_FLOAT, samples);
    } // !update audio

    { // window function
        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
        glUseProgram(s_program_hanning);
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_texture_audio_buffer);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    } // !window function
    
    { // bit reverse
        read_idx = write_idx;
        write_idx = 1 - write_idx;
        
        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
        glUseProgram(s_program_bit_reverse);
       
        glUniform1f(s_uniform_bit_reverse_gain, s_input_gain);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[read_idx]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, s_texture_bit_reverse_LUT);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    } // !bit reverse

    { // fft
        glUseProgram(s_program_fft);
        glUniform1i(s_uniform_fft_inverse, 0);
        glUniform1f(s_uniform_fft_buffer_size, (float)num_samples);
        
        for (int stage = 0; stage < 11; ++stage) {
            read_idx = write_idx;
            write_idx = 1 - write_idx;

            glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[read_idx]);
            
            glUniform1i(s_uniform_fft_stage, stage);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }       
    } // !fft

    { // eq
        read_idx = write_idx;
        write_idx = 1 - write_idx;

        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
        glUseProgram(s_program_eq);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[read_idx]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, s_texture_eq_curve_LUT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    } // !eq
    
    { // bit reverse
        read_idx = write_idx;
        write_idx = 1 - write_idx;
        
        glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
        glUseProgram(s_program_bit_reverse);
       
        glUniform1f(s_uniform_bit_reverse_gain, s_output_gain / num_samples);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[read_idx]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, s_texture_bit_reverse_LUT);
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    } // !bit reverse

    { // ifft
        glUseProgram(s_program_fft);
        glUniform1i(s_uniform_fft_inverse, 1);
        glUniform1f(s_uniform_fft_buffer_size, (float)num_samples);
        
        for (int stage = 0; stage < 11; ++stage) {
            read_idx = write_idx;
            write_idx = 1 - write_idx;

            glBindFramebuffer(GL_FRAMEBUFFER, s_framebuffers[write_idx]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, s_texture_framebuffers[read_idx]);
            
            glUniform1i(s_uniform_fft_stage, stage);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    } // !ifft

    glUseProgram(GL_NONE);

    float* temp_buffer = (float*)malloc(num_samples * num_channels * 2 * sizeof(float));
    glReadPixels(0, 0, num_samples, num_channels, GL_RG, GL_FLOAT, temp_buffer);
    for (int i = 0; i < num_samples * num_channels; ++i) {
        samples[i] = temp_buffer[i * 2];
    }
    free(temp_buffer);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*----------------------------------------------------------------------------*/

float dsp_get_input_gain(void) { return s_input_gain; }

/*----------------------------------------------------------------------------*/

float dsp_get_output_gain(void) { return s_output_gain; }

/*----------------------------------------------------------------------------*/

float dsp_get_eq_band_gain(uint8_t eq_band_idx) {
    return eq_band_idx >= 10 ? 0.0 : s_eq_bands[eq_band_idx];
}

/*----------------------------------------------------------------------------*/

void dsp_set_input_gain(float gain) { s_input_gain = gain; }

/*----------------------------------------------------------------------------*/

void dsp_set_output_gain(float gain) { s_output_gain = gain; }

/*----------------------------------------------------------------------------*/

void dsp_set_eq_band_gain(float eq_band_gain, uint8_t eq_band_idx) {
    if (eq_band_idx >= 10) { return; }
    s_eq_bands[eq_band_idx] = eq_band_gain;

    for (int i = 0; i < 10; ++i) {
        printf("%.2f ", s_eq_bands[i]);
    }
    printf("\n");

    glBindTexture(GL_TEXTURE_2D, s_texture_eq_curve_LUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 10, 1, 0, GL_RED, GL_FLOAT, s_eq_bands);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

/*----------------------------------------------------------------------------*/