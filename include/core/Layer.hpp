#ifndef CORE_LAYER_HPP
#define CORE_LAYER_HPP
#pragma once

#include "core/Event.hpp"

namespace gpudsp::core {

class Layer {
public:

    Layer(void);
    virtual ~Layer(void) {}
    virtual void onUpdate(void) = 0;
    virtual void onRender(void) = 0;
    virtual void onEvent(Event* event) = 0;

};

}

#endif