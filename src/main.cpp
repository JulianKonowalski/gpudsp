#include "core/Window.hpp"

int main(void) {
    gpudsp::core::Window* window = gpudsp::core::Window::getInstance();
    window->makeCurrent();
    while(!window->shouldClose()) { window->update(); }
}