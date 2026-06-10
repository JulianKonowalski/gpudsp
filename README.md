# GPU DSP

This is the code for my bachelor's project "Digital signal processing using
graphics processing units in the context of real-time audio". It's an audio
player with basic audio FX capabilities that demonstrates the ability to process
audio (aka signals) in parallel. This is more a POC project rather than an
industry-grade product, so it's not really optimised for high performance. If
you're after that go check out [GPU Audio](https://www.gpu.audio/).

## Setup
To clone the project use
```
git clone --recurse-submodules https://github.com/JulianKonowalski/gpudsp.git
cd gpudsp
```
then configure and build using
```
cd core
3rdparty/emsdk/emsdk download latest
3rdparty/emsdk/emsdk activate latest
emcmake cmake -G Ninja -B ./build -S .
cmake --build build
cd ../frontend
npm install
npm run start
```

This set of commands will download and activate the latest Emscripten toolchain
and allow you to compile the C side to wasm. CMake already has the post-build
scripts to copy the built files into the frontend directory. The only thing left
is to install the npm packages and run the webapp locally!