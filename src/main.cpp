#include "core/App.hpp"

int main(void) {
    gpudsp::core::App* app = gpudsp::core::App::getInstance();
    app->run();
}