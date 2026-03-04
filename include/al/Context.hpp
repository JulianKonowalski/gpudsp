#ifndef AL_CONTEXT_HPP 
#define AL_CONTEXT_HPP
#pragma once

#include <vector>
#include <cstdint>

#include "al/Device.hpp"

namespace gpudsp::al {

/*----------------------------------------------------------------------------*/

class Context final {
public:

    struct ContextParameters {

        /**
         * This looks weird, but both clang and gcc 
         * will throw errors when using the default
         * constructor. It's a known bug.
         */
        inline static ContextParameters Default(void) { return {}; }

        uint16_t sample_rate = 0;
        uint16_t mono_sources = 0;
        uint16_t refresh_rate = 0;
        uint16_t stereo_sources = 0;
        bool is_synchronous = false;
    };

    static Context* getCurrentContext(void);

    Context(Device& device, const ContextParameters& parameters = ContextParameters::Default());
    ~Context(void);

    inline Device& getContextsDevice(void) { return *m_device; }
    
    void bind(void);
    void unbind(void);
    void process(void);
    void suspend(void);

private:

    static std::vector<Context*> s_contexts;

    void* m_alc_context;
    Device* m_device;

};

/*----------------------------------------------------------------------------*/

}

#endif