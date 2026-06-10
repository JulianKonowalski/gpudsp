#version 300 es
precision highp float;

#define PI 3.141592653589793

/*----------------------------------------------------------------------------*/

uniform int         u_fft_stage;
uniform bool        u_fft_inverse;
uniform float       u_fft_buffer_size;
uniform sampler2D   u_buffer_input;

/*----------------------------------------------------------------------------*/

in vec2 v_texture_coordinates;
out vec2 output_sample;

/*----------------------------------------------------------------------------*/

void main() {
    float step = pow(2.0, float(u_fft_stage));
    float group_size = step * 2.0;

    float current_x_screenspace = floor(gl_FragCoord.x);
    vec2 current_sample = texelFetch(u_buffer_input, ivec2(gl_FragCoord), 0).xy;

    float angle = u_fft_inverse ? -1.0 : 1.0;
    vec2 twiddle = vec2(0.0);
    vec2 result = vec2(0.0);

    vec2 partner_sample = vec2(0.0);
    float partner_x_screenspace = 0.0;

    if (mod(current_x_screenspace, group_size) >= step) {
        partner_x_screenspace = current_x_screenspace - step;
        partner_sample = texelFetch(u_buffer_input, ivec2(int(partner_x_screenspace), gl_FragCoord.y), 0).xy;

        angle *= -2.0 * PI * mod(current_x_screenspace, step) / group_size;
        twiddle = vec2(cos(angle), sin(angle));
        
        result = partner_sample - vec2(
            current_sample.x * twiddle.x - current_sample.y * twiddle.y,
            current_sample.x * twiddle.y + current_sample.y * twiddle.x
        );
    } else {
        partner_x_screenspace = current_x_screenspace + step;
        partner_sample = texelFetch(u_buffer_input, ivec2(int(partner_x_screenspace), gl_FragCoord.y), 0).xy;
        
        angle *= -2.0 * PI * mod(partner_x_screenspace, step) / group_size;
        twiddle = vec2(cos(angle), sin(angle));
        
        result = current_sample + vec2(
            partner_sample.x * twiddle.x - partner_sample.y * twiddle.y,
            partner_sample.x * twiddle.y + partner_sample.y * twiddle.x
        );
    }

    output_sample = result;
}

/*----------------------------------------------------------------------------*/