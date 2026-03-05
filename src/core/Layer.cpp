#include "core/Layer.hpp"

#include "core/App.hpp"

using namespace gpudsp::core;

/*----------------------------------------------------------------------------*/

Layer::Layer(void) {
    if (!App::exists()) {
        throw std::runtime_error("The app must be created before any layers.");
    }
}

/*----------------------------------------------------------------------------*/
