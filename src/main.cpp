#include <AL/al.h>
#include <AL/alc.h>
#include <AL/salad.h>

int main(void) {
    saladLoadALdefault();
    ALCdevice* device = alcOpenDevice(nullptr);
    return 0;
}