#include <string>

#include "core/App.hpp"

#include "wav/WAVReader.hpp"
#include "layers/AudioPlayer.hpp"

static const std::string s_assets_path(ASSETS_PATH);

int main(int argc, char** argv) {
    gpudsp::wav::WAVReader reader(s_assets_path + std::string("solo.wav"));

    gpudsp::core::App* app = gpudsp::core::App::getInstance();
    app->pushLayer(new gpudsp::layers::AudioPlayer(reader.getSamples(), reader.getSpecificaiton().num_samples));
    app->run();
}