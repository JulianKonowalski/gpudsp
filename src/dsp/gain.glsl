#version 430 core

layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer DataBuffer {
    float data[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    data[2 * idx] = data[2 * idx] * 0.5;
    data[(2 * idx) + 1] = data[(2 * idx) + 1] * 0.5;
}