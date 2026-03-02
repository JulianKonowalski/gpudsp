#include "al/Context.hpp"

#include <vector>
#include <stdexcept>

#include <AL/al.h>
#include <AL/alc.h>

using namespace gpudsp::al;

/*----------------------------------------------------------------------------*/

Context* Context::s_current = nullptr;

/*----------------------------------------------------------------------------*/

Context::Context(
    Device& device,
    const ContextParameters& parameters
) : m_alc_context(nullptr),
    m_device(&device)
{
    /** 
     * OpenAL takes in the parameters
     * in the form of a 0-terminated
     * key-value pair array. The code
     * doesn't need to be fast, so
     * std::vector is used for convenience.
     */
    std::vector<ALint> paramsv;
    if (parameters.sample_rate)     { paramsv.push_back(ALC_FREQUENCY);         paramsv.push_back(parameters.sample_rate);      }
    if (parameters.mono_sources)    { paramsv.push_back(ALC_MONO_SOURCES);      paramsv.push_back(parameters.mono_sources);     }
    if (parameters.refresh_rate)    { paramsv.push_back(ALC_REFRESH);           paramsv.push_back(parameters.refresh_rate);     }
    if (parameters.stereo_sources)  { paramsv.push_back(ALC_STEREO_SOURCES);    paramsv.push_back(parameters.stereo_sources);   }
    paramsv.push_back(ALC_SYNC);
    paramsv.push_back(parameters.is_synchronous ? AL_TRUE : AL_FALSE);
    paramsv.push_back(0);

    if (!(m_alc_context = alcCreateContext((ALCdevice*)device.m_alc_device, paramsv.data()))) {
        switch(alcGetError((ALCdevice*)m_device->m_alc_device)) {
            case ALC_INVALID_VALUE: 
                throw std::runtime_error(
                    "An additional context can not be created for this device"
                );
            break;
            case ALC_INVALID_DEVICE:
                throw std::runtime_error(
                    "The specified device is not a valid output device"
                );
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/

Context::~Context(void) {
    if (alcGetCurrentContext() == m_alc_context) { alcMakeContextCurrent(nullptr); }
    alcDestroyContext((ALCcontext*)m_alc_context);
    alcGetError((ALCdevice*)m_device->m_alc_device); // clear any error codes
}

/*----------------------------------------------------------------------------*/

void Context::makeCurrent(void) {
    if (!alcMakeContextCurrent((ALCcontext*)m_alc_context)) {
        throw std::runtime_error("The specified context is invalid");
    }
    s_current = this;
}

/*----------------------------------------------------------------------------*/

void Context::process(void) {
    alcProcessContext((ALCcontext*)m_alc_context);
    if (alcGetError((ALCdevice*)m_device->m_alc_device) != ALC_NO_ERROR) {
        throw std::runtime_error("The specified context is invalid");
    }
}

/*----------------------------------------------------------------------------*/

void Context::suspend(void) {
    alcSuspendContext((ALCcontext*)m_alc_context);
    if (alcGetError((ALCdevice*)m_device->m_alc_device) != ALC_NO_ERROR) {
        throw std::runtime_error("The specified context is invalid");
    }
}

/*----------------------------------------------------------------------------*/