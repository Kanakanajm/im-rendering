#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(location = 0)
out vec3 color;

layout(scalar, buffer_reference) buffer VertexBuffer {
    vec3 vertices[36];
    vec3 vertexColors[36];
};

layout(scalar, buffer_reference) buffer DebugBuffer {
    vec4 vectors[400*400];
};

layout(scalar, buffer_reference) buffer TransformBuffer {
    mat4 mpp; // perspective projection matrix
    mat4 m_cs_ws; // camera rotation matrix
    mat4 m_cs_ws_rot; // camera space to world space, rotation only
};

layout(scalar, buffer_reference) buffer BlockBuffer {
    uint blocks[3][3][3];
};

layout(scalar, push_constant) uniform T {
	VertexBuffer vertex_buffer;
    DebugBuffer debug_buffer;
    DebugBuffer debug2_buffer;
    TransformBuffer trans_buffer;
    BlockBuffer block_buffer;
    ivec4 cube; // (x, y, z) position and id as w
} push_constants;

void main() {
    mat4 matrix = push_constants.trans_buffer.mpp;
    vec3 vertex = push_constants.vertex_buffer.vertices[gl_VertexIndex];
    gl_Position = matrix * vec4(vertex + push_constants.cube.xyz, 1.0);
    color = push_constants.vertex_buffer.vertexColors[gl_VertexIndex];
}