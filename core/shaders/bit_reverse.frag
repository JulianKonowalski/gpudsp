#version 300 es
precision highp float;

/*----------------------------------------------------------------------------*/

uniform float u_gain;
uniform sampler2D u_buffer_input;
uniform sampler2D u_lut_bit_reverse;

/*----------------------------------------------------------------------------*/

in vec2 v_texture_coordinates;
out vec2 output_sample;

/*----------------------------------------------------------------------------*/

void main() {
    ivec2 frag_coord = ivec2(gl_FragCoord.xy);

    /**
     * Honestly, I have no idea why does it have to be divided by 32... If you
     * omit the division, only every 32nd sample will be non-zero. Everything
     * else will be zeroed-out but no samples will be shifted in time, so the
     * problem doesn't even make sense. I've spent over a day trying to track
     * down the problem and everything looks fine. It shouldn't be a format
     * mismatch, as the LUT is passed in a form of a 32-bit floating point,
     * one-channel texture, so casting it down to an int for a texel fetch call
     * here shouldn't be a problem. Maybe if I find more time, I'll fix it, but
     * it'll probably just stay like that forever.
     */ 
    int reversed_idx = int(texelFetch(u_lut_bit_reverse, ivec2(frag_coord.x, 0), 0).x) / 32;
    output_sample = texelFetch(u_buffer_input, ivec2(reversed_idx, frag_coord.y), 0).xy * u_gain;
}

/*----------------------------------------------------------------------------*/