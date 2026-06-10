#version 300 es
precision highp float;

#define PI 3.141592653589793

/*----------------------------------------------------------------------------*/

uniform sampler2D u_buffer_input;

/*----------------------------------------------------------------------------*/

in vec2 v_texture_coordinates;
out vec2 output_sample;

/*----------------------------------------------------------------------------*/

void main() {
    output_sample = vec2(texture(u_buffer_input, v_texture_coordinates).x
        * (0.5 * (1.0 - cos(2.0 * PI * v_texture_coordinates.x))), 0.0);
}

/*----------------------------------------------------------------------------*/