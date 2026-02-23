#ifndef AUDIO_DEVICE_HPP
#define AUDIO_DEVICE_HPP
#pragma once

#include <string>
#include <vector>

namespace gpudsp::audio {

/*----------------------------------------------------------------------------*/

class Device final {

    friend class Context;

public:

    static std::vector<std::string> getAvailableDevices(void);

    Device(const std::string& device_name = "");
    ~Device(void);

private:

    void* m_alc_device;

};

/*----------------------------------------------------------------------------*/

}

#endif