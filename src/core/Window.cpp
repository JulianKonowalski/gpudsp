#include "core/Window.hpp"

#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace gpudsp::core;

/*----------------------------------------------------------------------------*/

void glfwErrorCallback(int error, const char* error_message) {}

void glfwKeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {}

void glfwMouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods) {}
void glfwMouseScrollCallback(GLFWwindow* glfw_window, double x_offset, double y_offset) {}
void glfwMouseCursorPosCallback(GLFWwindow* glfw_window, double x_pos, double y_pos) {}

void glfwFramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {}

/*----------------------------------------------------------------------------*/

Window* Window::s_instance = nullptr;

/*----------------------------------------------------------------------------*/

Window* Window::getInstance(const WindowParameters& parameters) {
    if (!s_instance) { s_instance = new Window(parameters); }
    return s_instance;
}

/*----------------------------------------------------------------------------*/

void Window::update(void) { 
    glfwPollEvents();
    glfwSwapBuffers((GLFWwindow*)m_glfw_window); 
}

/*----------------------------------------------------------------------------*/

void Window::makeCurrent(void) { glfwMakeContextCurrent((GLFWwindow*)m_glfw_window); }

/*----------------------------------------------------------------------------*/

bool Window::shouldClose(void) { return glfwWindowShouldClose((GLFWwindow*)m_glfw_window); }

/*----------------------------------------------------------------------------*/

Window::Window(
    const WindowParameters& parameters 
) : m_glfw_window(nullptr),
    m_parameters(parameters)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) { throw std::runtime_error("Failed to initialize GLFW"); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    m_glfw_window = glfwCreateWindow(
        m_parameters.size[0],
        m_parameters.size[1],
        m_parameters.title,
        nullptr, nullptr
    );

    if (!m_glfw_window) { throw std::runtime_error("Failed to create GLFW window"); }
    
    glfwMakeContextCurrent((GLFWwindow*)m_glfw_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load OpenGL functions");        
    }

    glfwSetWindowUserPointer((GLFWwindow*)m_glfw_window, this);
    glfwSetKeyCallback((GLFWwindow*)m_glfw_window, glfwKeyCallback);
    glfwSetScrollCallback((GLFWwindow*)m_glfw_window, glfwMouseScrollCallback);
    glfwSetMouseButtonCallback((GLFWwindow*)m_glfw_window, glfwMouseButtonCallback);
    glfwSetCursorPosCallback((GLFWwindow*)m_glfw_window, glfwMouseCursorPosCallback);
    glfwSetFramebufferSizeCallback((GLFWwindow*)m_glfw_window, glfwFramebufferSizeCallback);

    glfwMakeContextCurrent(nullptr);
}

/*----------------------------------------------------------------------------*/

Window::~Window(void) { glfwDestroyWindow((GLFWwindow*)m_glfw_window); }

/*----------------------------------------------------------------------------*/