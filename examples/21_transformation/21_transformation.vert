#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(location = 0)
out vec3 color;

layout(scalar, buffer_reference) buffer VertexBuffer {
    vec2 vertices[18];
};

layout(scalar, push_constant) uniform T {
	VertexBuffer vertex_buffer;
    ivec2 resolution;
} push_constants;

void main() {
    vec2 position = push_constants.vertex_buffer.vertices[gl_VertexIndex];
    position = 2 * position / push_constants.resolution - 1;
    gl_Position = vec4(position, 0.5, 1);
    color = vec3(1, 0, 0);
}