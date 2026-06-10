#version 300 es
precision highp float;

out vec2 v_texture_coordinates;

void main(void) {
    vec2 vertex_positions[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0,  1.0),
        vec2( 1.0,  1.0)
    );
    vec2 vertex_position = vertex_positions[gl_VertexID];
    v_texture_coordinates = (vertex_position + 1.0) * 0.5;
    gl_Position = vec4(vertex_position, 0.0, 1.0);
}