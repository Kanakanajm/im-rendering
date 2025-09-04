#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(location = 0)
in vec3 color;

layout(location = 0)
out vec4 colorOut;

layout(scalar, buffer_reference) buffer VertexBuffer {
    vec3 vertices[36];
    vec3 vertexColors[36];
};

layout(scalar, buffer_reference) buffer DebugBuffer {
    vec4 vectors[400*400];
};

layout(scalar, buffer_reference) buffer TransformBuffer {
    mat4 mpp; // prospective projection matrix
    mat4 mr; // camera rotation matrix
    mat4 mpp_inv;
    vec3 cam_pos;
};

layout(scalar, push_constant) uniform T {
	VertexBuffer vertex_buffer;
    DebugBuffer debug_buffer;
    DebugBuffer debug2_buffer;
    TransformBuffer trans_buffer;
    ivec4 cube; // (x, y, z) position and id as w
} push_constants;

bool inRange(ivec3 m) {
    return all(greaterThanEqual(m, ivec3(0))) && all(lessThan(m, ivec3(3)));
}

bool textureFrontFace(ivec3 m) {
    return inRange(m) && m.z == 2;
}

float palette[3] = {0.3, 0.6, 0.9};

// bool textureCross(ivec3 m) {
//     return inRange(m) && m 
// }

void main() {
    vec2 screen = gl_FragCoord.xy - vec2(0.5);

    ivec2 iscreen = ivec2(screen); // skip this

    vec2 ndc = screen / vec2(200) - vec2(1); // [-1, 1], 2 * (screen / image.xy) - 1
    ndc.y = -ndc.y;

    float depth = 1 / gl_FragCoord.w; // depth to the camera not the near plane

    vec3 cs = vec3(depth * ndc, -depth);

    vec4 ws = push_constants.trans_buffer.mr * vec4(cs, 1); // camera translation

    vec4 os = ws - vec4(vec3(push_constants.cube.xyz), 0);

    // debug saves
    push_constants.debug_buffer.vectors[iscreen.y*400 + iscreen.x] = gl_FragCoord;
    push_constants.debug2_buffer.vectors[iscreen.y*400 + iscreen.x] = os;

    ivec3 map = ivec3(os.xyz);

    colorOut = vec4(palette[map.x], palette[map.y], palette[map.z], float(inRange(map)));
}


