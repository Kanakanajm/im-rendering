#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require
#define EPSILON 1e-3
#define EPSILON_FACE 1e-3
#define MAX_STEP 5
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
    mat4 mpp; // perspective projection matrix, world space -> clip space
    mat4 mpp_inv; // inverse of perspective projection matrix, clip space -> world space
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
    float rotate_rad;
} push_constants;

bool inRange(ivec3 m) {
    return all(greaterThanEqual(vec3(m), vec3(0))) && all(lessThan(vec3(m), vec3(3)));
}

bool isBlock(ivec3 m) {
    return inRange(m) && push_constants.block_buffer.blocks[m.x][m.y][m.z] != 0;
}

// should only be used in dda after inRange check!
// vec4 blockColor(ivec3 m) {
//     // buffered
//     if (push_constants.block_buffer.blocks[m.x][m.y][m.z]) {
//         return vec4(1);
//     }    
//     // procedure
//     // if (inRange(m)) {
//     //     return vec4(1);
//     // }
//     return vec4(0);
// }

bool textureFrontFace(ivec3 m) {
    return inRange(m) && m.z == 2;
}



float palette[3] = {0.3, 0.6, 0.9};

void main() {

    mat4 rotateYInv = mat4(mat3(
        cos(-push_constants.rotate_rad), 0.0, sin(-push_constants.rotate_rad),
        0.0, 1.0, 0.0,
        -sin(-push_constants.rotate_rad), 0.0, cos(-push_constants.rotate_rad)
    ));

    vec2 screen = gl_FragCoord.xy - vec2(0.5);
    ivec2 iscreen = ivec2(screen);
    screen = screen / vec2(200) - 1;
    vec4 clip_space = vec4(screen, gl_FragCoord.z, 1);

    vec4 world_space = push_constants.trans_buffer.mpp_inv * clip_space;
    world_space /= world_space.w;

    vec4 object_space = rotateYInv * (world_space - vec4(push_constants.cube.xyz, 0));

    // gl_FragDepth = 1 - gl_FragCoord.z;
        // // debug saves
    push_constants.debug_buffer.vectors[iscreen.y*400 + iscreen.x] = gl_FragCoord;
    push_constants.debug2_buffer.vectors[iscreen.y*400 + iscreen.x] = world_space;

    // object space
    vec3 pos = object_space.xyz;
    // clamp to [0, 3)
    pos = clamp(pos, 0, 2.9999);

    ivec3 map = ivec3(pos);

    // show bounding box color
    // colorOut = vec4(color, 1);

    // show dir debug
    // colorOut = vec4(dir, 1.0);

    // show coverage
    // colorOut = vec4(float(inRange(map)));

    // show object space index
    colorOut = vec4(palette[map.x], palette[map.y], palette[map.z], 1) * float(inRange(map));

    // show front face only
    // colorOut = vec4(float(textureFrontFace(map)));

    // dda
    // vec3 deltaDist = abs(1 / dir);

    // ivec3 rayStep = ivec3(sign(dir));

    // vec3 sideDist =
    //     (sign(dir) * (vec3(map) - pos) + (sign(dir) * 0.5) + 0.5) * deltaDist;




    // bvec3 mask;
    // if (abs(pos.x - 0.0) < EPSILON_FACE || abs(pos.x - 3.0) < EPSILON_FACE) {
    //      mask = bvec3(true, false, false);
    // } else if (abs(pos.y - 0.0) < EPSILON_FACE || abs(pos.y - 3.0) < EPSILON_FACE) {
    //     mask = bvec3(false, true, false);
    // } else if (abs(pos.z - 0.0) < EPSILON_FACE || abs(pos.z - 3.0) < EPSILON_FACE) {
    //     mask = bvec3(false, false, true);
    // }

    
    // for (int i = 0; i < MAX_STEP; i++) {
    //     // if hit block
    //     if (isBlock(map)) {
    //         float shadow;
    //         // fake shadow on sides
    //         if (mask.x) {
    //             shadow = 0.5;
    //         }
    //         if (mask.y) {
    //             shadow = 1.0;
    //         }
    //         if (mask.z) {
    //             shadow = 0.75;
    //         }
    //         // mix shadow color with block color
    //         colorOut = shadow * vec4(push_constants.block_buffer.blocks[map.x][map.y][map.z]);

    //         return;
    //     }

    //     if (sideDist.x < sideDist.y) {
    //         if (sideDist.x < sideDist.z) {
    //             sideDist.x += deltaDist.x;
    //             map.x += rayStep.x;
    //             mask = bvec3(true, false, false);
    //         } else {
    //             sideDist.z += deltaDist.z;
    //             map.z += rayStep.z;
    //             mask = bvec3(false, false, true);
    //         }
    //     } else {
    //         if (sideDist.y < sideDist.z) {
    //             sideDist.y += deltaDist.y;
    //             map.y += rayStep.y;
    //             mask = bvec3(false, true, false);
    //         } else {
    //             sideDist.z += deltaDist.z;
    //             map.z += rayStep.z;
    //             mask = bvec3(false, false, true);
    //         }
    //     }
    // }

    // discard;

}


