#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(set = 0, binding = 0)
uniform image2D renderTarget;

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(scalar, buffer_reference) buffer MapBuffer {
    int map[24][24];
};
layout(scalar, push_constant) uniform T {
	MapBuffer map_buffer;
    vec2 pos;
    vec2 dir;
    vec2 plane;
} push_constants;

void main() {
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 img_size = imageSize(renderTarget);
    if (gl_GlobalInvocationID.x >= img_size.x || gl_GlobalInvocationID.y >= img_size.y)
        return;

    vec4 c = vec4(0.0);

    vec2 bin = vec2(gl_GlobalInvocationID.xy) / vec2(img_size.xy) * 24.0;
    
    ivec2 map_coord = {int(bin.x), int(bin.y)};

    int block = push_constants.map_buffer.map[map_coord.x][map_coord.y];
    
    // block type
    switch (block) {
        case 1:  c = vec4(0.5, 0.5, 0.5, 1.0);    break; //grey
        case 2:  c = vec4(0.0, 1.0, 0.0, 1.0);  break; //green
        case 3:  c = vec4(0.0, 0.0, 1.0, 1.0);   break; //blue
        case 4:  c = vec4(1.0, 1.0, 1.0, 1.0);  break; //white
        case 5:  c = vec4(1.0, 0.0, 0.0, 1.0); break; //red
        default: c = vec4(0.0, 0.0, 0.0, 1.0); break; // black
    }

    vec2 arrow_head = push_constants.pos + push_constants.dir * 1.5f;
    

    if (map_coord.x == int(push_constants.pos.x) && map_coord.y == int(push_constants.pos.y)) {
        // player position
        c = vec4(1.0, 0.0, 0.0, 1.0); 
    }

    if (map_coord.x == int(arrow_head.x) && map_coord.y == int(arrow_head.y)) {
        // player direction
        c = vec4(1.0, 0.5, 0.0, 1.0); 
    }

    ivec2 dst = ivec2(pix.x, img_size.y - 1 - pix.y);

    imageStore(renderTarget, dst, c);
}