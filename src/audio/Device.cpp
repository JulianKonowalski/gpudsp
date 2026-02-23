#include "audio/Device.hpp"

#include <stdexcept>

#include <AL/alc.h>

using namespace gpudsp::audio;

/*----------------------------------------------------------------------------*/

std::vector<std::string> Device::getAvailableDevices(void) {
    if (!alcIsExtensionPresent(nullptr, (const ALCchar*)"ALC_ENUMERATION_EXT")) { return {}; }

    size_t idx = 0;
    std::vector<std::string> names;
    const char* devices = (const char*)alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
 
    for(;;) { // names are separated by \0, string is terminated by \0\0
        if (devices[idx] == '\0') { break; }
        names.push_back(&devices[idx]);
        idx += names.back().length() + 1;
    }

    return std::move(names);
}

/*----------------------------------------------------------------------------*/

Device::Device(
    const std::string& device_name
) : m_alc_device(nullptr) 
{
    const char* p_device_name = device_name.empty() ? 
        nullptr : device_name.c_str();

    if (!(m_alc_device = alcOpenDevice((const ALCchar*)p_device_name))) { 
        throw std::runtime_error("Failed to create an audio device"); 
    }
}

/*----------------------------------------------------------------------------*/

Device::~Device(void) { 
    if (!alcCloseDevice((ALCdevice*)m_alc_device)) {
        alcGetError((ALCdevice*)m_alc_device); // clear any error codes
    }
}

/*----------------------------------------------------------------------------*/