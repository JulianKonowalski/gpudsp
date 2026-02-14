#include <glad/glad.h>
#include <glfw/glfw3.h>

int main(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(300, 300, "Test Window", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glViewport(0, 0, 200, 300);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);

        glfwSwapBuffers(window);
    }
}