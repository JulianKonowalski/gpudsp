#include "core/App.hpp"

#include <cstring>
#include <functional>

using namespace gpudsp::core;

/*----------------------------------------------------------------------------*/

App* App::s_instance = nullptr;

/*----------------------------------------------------------------------------*/

App* App::getInstance(const AppParameters& parameters) {
    if (!s_instance) { s_instance = new App(parameters); }
    return s_instance;
}

/*----------------------------------------------------------------------------*/

void App::run(void) {
    m_audio->start();
    m_window->makeCurrent();
    while(!m_window->shouldClose()) {
        m_window->update();

        while (!m_event_queue.empty()) { 
            Event* event = m_event_queue.front();
            m_event_queue.pop();

            for (Layer* layer : m_layers) {
                layer->onEvent(event);
                if (event->isHandled()) { break; }
            }

            delete event;
        }

        for (Layer* layer : m_layers) {
            layer->onUpdate();
            layer->onRender();
        }
    }
}

/*----------------------------------------------------------------------------*/

App::App(
    const AppParameters& parameters
) : m_audio(Audio::getInstance({
        std::bind(&App::audioCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        parameters.audio_sample_rate,
        2048,
        false
    })),
    m_window(Window::getInstance({
        std::bind(&App::eventCallback, this, std::placeholders::_1),
        "GPU DSP",
        { parameters.screen_width, parameters.screen_height }
    })),
    m_audio_buffer(parameters.audio_chunk_size, parameters.audio_num_chunks) {}

/*----------------------------------------------------------------------------*/

App::~App(void) {
    for (Layer* layer : m_layers) { delete layer; }
}

/*----------------------------------------------------------------------------*/

void App::audioCallback(
    float* buffer,
    uint16_t buffer_size,
    uint8_t num_channels
) {
    if (!m_audio_buffer.readable()) { return; }
    m_audio_buffer.readChunk(buffer);
}

/*----------------------------------------------------------------------------*/

void App::eventCallback(Event* event) { m_event_queue.push(event); }

/*----------------------------------------------------------------------------*/