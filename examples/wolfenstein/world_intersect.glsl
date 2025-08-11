#version 450
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_buffer_reference : require

layout(local_size_x = 32) in;

layout(scalar, buffer_reference) buffer MapBuffer { int map[24][24]; };

layout(scalar, buffer_reference) buffer ItsBuffer { ivec2 its[3840]; };

layout(scalar, buffer_reference) buffer DistBuffer { float dist[3840]; };

layout(scalar, push_constant) uniform T {
  MapBuffer map_buffer;
  ItsBuffer its_buffer;
  DistBuffer dist_buffer;
  vec2 pos;
  vec2 dir;
  vec2 plane;
}
push_constants;

void main() {
  // block on map
  int mapX = int(push_constants.pos.x);
  int mapY = int(push_constants.pos.y);

  // x-coordinate in camera space, in [-1, 1)
  float cameraX = 2.0 * float(gl_GlobalInvocationID.x) / 3840.0 - 1.0;

  vec2 ray = push_constants.dir + push_constants.plane * cameraX;

  float deltaDistX = ray.x == 0 ? 1e30 : abs(1 / ray.x);
  float deltaDistY = ray.y == 0 ? 1e30 : abs(1 / ray.y);

  // length of ray from current position to next x or y-side
  float sideDistX;
  float sideDistY;

  // distance to the wall
  float perpWallDist;

  // what direction to step in x or y-direction (either +1 or -1)
  int stepX;
  int stepY;
  int hit = 0; // was there a wall hit?
  int side;    // was a NS or a EW wall hit?

  if (ray.x < 0) {
    stepX = -1;
    sideDistX = (push_constants.pos.x - mapX) * deltaDistX;
  } else {
    stepX = 1;
    sideDistX = (mapX + 1.0 - push_constants.pos.x) * deltaDistX;
  }
  if (ray.y < 0) {
    stepY = -1;
    sideDistY = (push_constants.pos.y - mapY) * deltaDistY;
  } else {
    stepY = 1;
    sideDistY = (mapY + 1.0 - push_constants.pos.y) * deltaDistY;
  }
  // perform DDA
  int i = 0;
  while (hit == 0 && i < 100) {
    // jump to next map square, either in x-direction, or in y-direction
    if (sideDistX < sideDistY) {
      sideDistX += deltaDistX;
      mapX += stepX;
      side = 0;
    } else {
      sideDistY += deltaDistY;
      mapY += stepY;
      side = 1;
    }
    // Check if ray has hit a wall
    if (push_constants.map_buffer.map[mapX][mapY] > 0) {
      hit = 1;
      push_constants.its_buffer.its[gl_GlobalInvocationID.x].x =
          push_constants.map_buffer.map[mapX][mapY];
    }
    i++;
  }

  // where ray intersects wall in x-axis
  float wallX;
  if (side == 0) {
    perpWallDist = sideDistX - deltaDistX;
    wallX = push_constants.pos.y + perpWallDist * ray.y;
  } else {
    perpWallDist = sideDistY - deltaDistY;
    wallX = push_constants.pos.x + perpWallDist * ray.x;

    push_constants.its_buffer.its[gl_GlobalInvocationID.x].x += 6;
  }

  wallX -= floor((wallX));

  // texture x coordinate, 64 is texture size
  int texX = int(wallX * 64.0);

  // the order of texture should be the same (left to right)
  if (side == 0 && ray.x > 0)
    texX = 64 - texX - 1;
  if (side == 1 && ray.y < 0)
    texX = 64 - texX - 1;
  push_constants.its_buffer.its[gl_GlobalInvocationID.x].y = texX;

  if (hit == 0) {
    push_constants.its_buffer.its[gl_GlobalInvocationID.x].x = 0;
    push_constants.dist_buffer.dist[gl_GlobalInvocationID.x] = 0;

  } else {
    push_constants.dist_buffer.dist[gl_GlobalInvocationID.x] = perpWallDist;
  }
}
