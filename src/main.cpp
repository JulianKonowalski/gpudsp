#include <cmath>
#include <iostream>

#include "core/Audio.hpp"

int main(void) {
    float angle = 0.0f;
    const float offset = (2.0f * 3.1415f * 440.0f) / 44100.0f;
    gpudsp::core::Audio::Callback cb = [&angle, &offset] (
        float* buffer,
        uint16_t buffer_size,
        uint8_t num_channels
    ) {
        float sample = 0.0f;
        for (int i = 0; i < buffer_size * num_channels; i += num_channels) {
            sample = std::sinf(angle);
            angle += offset;
            angle -= angle < 2.0f * 3.1415f ? 0.0f : 2.0f * 3.1415f;
            for (int j = 0; j < num_channels; ++j) { buffer[i + j] = sample; }
        }
    };

    gpudsp::core::Audio* audio = gpudsp::core::Audio::getInstance(cb, 44100, 2048);
    audio->start();

    char c;
    std::cout << "Press any key to exit\n";
    std::cin >> c;
}