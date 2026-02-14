# GPU DSP

This is the code for my bachelor's project "Digital Signal Processing on Graphics Processing Units".
It's an audio player with basic audio FX capabilities that demonstrates the ability to process audio
(aka signals) in parallel. This is more a POC project rather than an industry-grade product, so it's
not really optimised for high performance. If you're after that go check out [GPU Audio](https://www.gpu.audio/).

## Setup
To clone the project use
```
git clone https://github.com/JulianKonowalski/gpudsp.git
```
then configure and build using
```
cd gpudsp
cmake -G <generator> -B ./build -S .
cmake --build build
```