#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(set = 0, binding = 0) uniform image2D renderTarget;

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(scalar, buffer_reference) buffer ItsBuffer { ivec2 its[3840]; };

layout(scalar, buffer_reference) buffer DistBuffer { float dist[3840]; };

layout(scalar, push_constant) uniform T {
  ItsBuffer its_buffer;
  DistBuffer dist_buffer;
}
push_constants;

void main() {
  ivec2 img_size = imageSize(renderTarget);

  if (push_constants.its_buffer.its[gl_GlobalInvocationID.x].x > 0) {

    // Calculate height of line to draw on screen
    int lineHeight = int(
        img_size.y / push_constants.dist_buffer.dist[gl_GlobalInvocationID.x]);

    // //calculate lowest and highest pixel to fill in current stripe
    int drawStart = -lineHeight / 2 + img_size.y / 2;
    if (drawStart < 0)
      drawStart = 0;
    int drawEnd = lineHeight / 2 + img_size.y / 2;
    if (drawEnd >= img_size.y)
      drawEnd = img_size.y - 1;

    vec4 c = vec4(0.0, 0.0, 0.0, 1.0);

    if (gl_GlobalInvocationID.y <= drawEnd &&
        gl_GlobalInvocationID.y >= drawStart) {
      switch (push_constants.its_buffer.its[gl_GlobalInvocationID.x].x % 6) {
      case 1:
        c = vec4(0.5, 0.5, 0.5, 1.0);
        break; // grey
      case 2:
        c = vec4(push_constants.its_buffer.its[gl_GlobalInvocationID.x].y % 2,
                 1.0, 0.0, 1.0);
        break; // green
      case 3:
        c = vec4(0.0, 0.0, 1.0, 1.0);
        break; // blue
      case 4:
        c = vec4(1.0, 1.0, 1.0, 1.0);
        break; // white
      case 5:
        c = vec4(1.0, 0.0, 0.0, 1.0);
        break; // red
      default:
        c = vec4(0.0, 0.0, 0.0, 1.0);
        break; // black
      }
      // give x and y sides different brightness
      if (push_constants.its_buffer.its[gl_GlobalInvocationID.x].x > 6) {
        c = c / 2;
      }
    }
    imageStore(renderTarget, ivec2(gl_GlobalInvocationID.xy), c);
  }
}