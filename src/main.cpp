#include <cmath>
#include <iostream>

#include <AL/salad.h>

#include "audio/Device.hpp"
#include "audio/Context.hpp"
#include "audio/Buffer.hpp"
#include "audio/Source.hpp"

int main(void) {
    if (!saladLoadALdefault()) { 
        std::cout << "Failed to load OpenAL functions.\n";
        exit(-1);
    }

    for (const auto& device : gpudsp::audio::Device::getAvailableDevices()) {
        std::cout << device << "\n";
    }

    std::vector<uint16_t> samples;
    samples.reserve(44100);
    double offset = (2 * 3.1415 * 440.0f) / 44100.0;
    for (int i = 0; i < 44100; ++i) { samples.push_back(sinf(i * offset) * 32767); }

    gpudsp::audio::Device device;
    gpudsp::audio::Context context(device);
    context.makeCurrent();

    gpudsp::audio::Buffer buffer;
    buffer.setData(samples, 44100, gpudsp::audio::Buffer::Type::MONO);

    gpudsp::audio::Source source;
    source.setLooping(true);
    source.attachBuffer(buffer);
    source.play();

    char c;
    std::cout << "Press any button to stop\n";
    std::cin >> c;
    
    source.stop();
}