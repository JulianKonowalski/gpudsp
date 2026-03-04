#include "core/App.hpp"

#include <functional>

using namespace gpudsp::core;

/*----------------------------------------------------------------------------*/

App* App::s_instance = nullptr;

/*----------------------------------------------------------------------------*/

App* App::getInstance(
    const Audio::AudioParameters& audio_parameters,
    const Window::WindowParameters& window_parameters
) {
    if (!s_instance) { s_instance = new App(audio_parameters, window_parameters); }
    return s_instance;
}

/*----------------------------------------------------------------------------*/

void App::run(void) {
    m_audio->start();
    m_window->makeCurrent();
    while(!m_window->shouldClose()) {
        m_window->update();
        // empty the event queue
        // render video & queue audio buffers
    }
}

/*----------------------------------------------------------------------------*/

App::App(
    const Audio::AudioParameters& audio_parameters,
    const Window::WindowParameters& window_parameters
) : m_audio(Audio::getInstance(audio_parameters)),
    m_window(Window::getInstance(window_parameters)) 
{
    m_audio->setCallback(std::bind(
        &App::audioCallback, this, 
        std::placeholders::_1, 
        std::placeholders::_2, 
        std::placeholders::_3
    ));
}

/*----------------------------------------------------------------------------*/

App::~App(void) {}

/*----------------------------------------------------------------------------*/

void App::audioCallback(
    float* buffer,
    uint16_t buffer_size,
    uint8_t num_channels
) {
    std::memset(buffer, 0, buffer_size * num_channels * sizeof(float)); // temporary
    // empty the internal ring buffer
}

/*----------------------------------------------------------------------------*/