#version 300 es
precision highp float;

/*----------------------------------------------------------------------------*/

uniform sampler2D u_buffer_input;
uniform sampler2D u_lut_eq_curve;

/*----------------------------------------------------------------------------*/

in vec2 v_texture_coordinates;
out vec2 output_sample;

/*----------------------------------------------------------------------------*/

void main() {
    float x_mirrored    = 1.0 - abs((v_texture_coordinates.x * 2.0) - 1.0);
    float x_mapped = sqrt(x_mirrored);
    float scalar = texture(u_lut_eq_curve, vec2(x_mapped, 0.0)).r;
    output_sample = texelFetch(u_buffer_input, ivec2(gl_FragCoord), 0).xy * scalar;
}

/*----------------------------------------------------------------------------*/