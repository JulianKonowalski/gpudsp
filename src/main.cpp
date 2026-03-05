#include "core/App.hpp"

#include "layers/AudioGenerator.hpp"

int main(void) {
    gpudsp::core::App* app = gpudsp::core::App::getInstance();
    app->pushLayer(new gpudsp::layers::AudioGenerator());
    app->run();
}