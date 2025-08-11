#include "imr/imr.h"
#include "imr/util.h"
#include "imr_math.h"

#include "VkBootstrap.h"

#include <ctime>
#include <filesystem>
#include <iostream>
#include <memory>

#define BLOCK_SPEED 3.0f    // number of block per second
#define ROTATE_SPEED 1.571f // radians per second, default as pi/8

#define WIN_WIDTH 3840
#define WIN_HEIGHT 2160

int worldMap[24][24] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct vec2 {
  float x, y;
};

struct ivec2 {
  int x, y;
};

struct {
  VkDeviceAddress map_buffer;
  vec2 pos;
  vec2 dir;
  vec2 plane;
} push_constants_map;

struct {
  VkDeviceAddress map_buffer;
  VkDeviceAddress its_buffer;
  VkDeviceAddress dist_buffer;
  vec2 pos;
  vec2 dir;
  vec2 plane;
} push_constants_intersect;

struct {
  VkDeviceAddress its_buffer;
  VkDeviceAddress dist_buffer;
} push_constants_render;

struct keys {
  bool forward, back, left, right;
} input;

struct Shaders {
  imr::ComputePipeline map;
  imr::ComputePipeline world_intersect;
  imr::ComputePipeline world_render;

  Shaders(imr::Device &d)
      : map(d, "map.spv"), world_intersect(d, "world_intersect.spv"),
        world_render(d, "world_render.spv") {}
};

float moveSpeed = 0.0001f;
float rotSpeed = 0.0001f;

bool showMap = true;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  auto window =
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "WS3D", nullptr, nullptr);

  imr::Context context;
  imr::Device device(context);
  imr::Swapchain swapchain(device, window);
  imr::FpsCounter fps_counter;

  auto shaders = std::make_unique<Shaders>(device);

  auto &vk = device.dispatch;

  std::unique_ptr<imr::Buffer> map_buffer = std::make_unique<imr::Buffer>(
      device, sizeof(worldMap),
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  map_buffer->uploadDataSync(0, map_buffer->size, worldMap);
  push_constants_intersect.map_buffer = map_buffer->device_address();

  ivec2 its[3840];
  std::unique_ptr<imr::Buffer> its_buffer = std::make_unique<imr::Buffer>(
      device, sizeof(its),
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  its_buffer->uploadDataSync(0, its_buffer->size, its);
  push_constants_intersect.its_buffer = its_buffer->device_address();
  push_constants_render.its_buffer = its_buffer->device_address();

  float dist[3840];
  std::unique_ptr<imr::Buffer> dist_buffer = std::make_unique<imr::Buffer>(
      device, sizeof(dist),
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
  dist_buffer->uploadDataSync(0, dist_buffer->size, dist);
  push_constants_intersect.dist_buffer = dist_buffer->device_address();
  push_constants_render.dist_buffer = dist_buffer->device_address();

  push_constants_intersect.dir = {-1, 0};
  push_constants_intersect.pos = {22, 12};
  push_constants_intersect.plane = {0, 0.66};

  while (!glfwWindowShouldClose(window)) {
    fps_counter.tick();
    fps_counter.updateGlfwWindowTitle(window);

    // actual move speed (block/frame)
    moveSpeed = BLOCK_SPEED * 0.00125; // 800 fps
    rotSpeed = ROTATE_SPEED * 0.00125; // 800 fps
    // Input stage
    // Read keys
    input.forward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    input.back = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    input.left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    input.right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    showMap = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;

    // forward & backward
    if (input.forward) {
      if (worldMap[int(push_constants_intersect.pos.x +
                       push_constants_intersect.dir.x * moveSpeed)]
                  [int(push_constants_intersect.pos.y)] == false)
        push_constants_intersect.pos.x +=
            push_constants_intersect.dir.x * moveSpeed;
      if (worldMap[int(push_constants_intersect.pos.x)]
                  [int(push_constants_intersect.pos.y +
                       push_constants_intersect.dir.y * moveSpeed)] == false)
        push_constants_intersect.pos.y +=
            push_constants_intersect.dir.y * moveSpeed;
    }
    if (input.back) {
      if (worldMap[int(push_constants_intersect.pos.x -
                       push_constants_intersect.dir.x * moveSpeed)]
                  [int(push_constants_intersect.pos.y)] == false)
        push_constants_intersect.pos.x -=
            push_constants_intersect.dir.x * moveSpeed;
      if (worldMap[int(push_constants_intersect.pos.x)]
                  [int(push_constants_intersect.pos.y -
                       push_constants_intersect.dir.y * moveSpeed)] == false)
        push_constants_intersect.pos.y -=
            push_constants_intersect.dir.y * moveSpeed;
    }
    // rotate
    if (input.right) {
      double oldDirX = push_constants_intersect.dir.x;
      push_constants_intersect.dir.x =
          push_constants_intersect.dir.x * cosf(-rotSpeed) -
          push_constants_intersect.dir.y * sinf(-rotSpeed);
      push_constants_intersect.dir.y =
          oldDirX * sinf(-rotSpeed) +
          push_constants_intersect.dir.y * cosf(-rotSpeed);
      double oldPlaneX = push_constants_intersect.plane.x;
      push_constants_intersect.plane.x =
          push_constants_intersect.plane.x * cosf(-rotSpeed) -
          push_constants_intersect.plane.y * sinf(-rotSpeed);
      push_constants_intersect.plane.y =
          oldPlaneX * sinf(-rotSpeed) +
          push_constants_intersect.plane.y * cosf(-rotSpeed);
    }
    if (input.left) {
      double oldDirX = push_constants_intersect.dir.x;
      push_constants_intersect.dir.x =
          push_constants_intersect.dir.x * cosf(rotSpeed) -
          push_constants_intersect.dir.y * sinf(rotSpeed);
      push_constants_intersect.dir.y =
          oldDirX * sinf(rotSpeed) +
          push_constants_intersect.dir.y * cosf(rotSpeed);
      double oldPlaneX = push_constants_intersect.plane.x;
      push_constants_intersect.plane.x =
          push_constants_intersect.plane.x * cosf(rotSpeed) -
          push_constants_intersect.plane.y * sinf(rotSpeed);
      push_constants_intersect.plane.y =
          oldPlaneX * sinf(rotSpeed) +
          push_constants_intersect.plane.y * cosf(rotSpeed);
    }

    push_constants_map.dir = push_constants_intersect.dir;
    push_constants_map.pos = push_constants_intersect.pos;
    push_constants_map.plane = push_constants_intersect.plane;
    push_constants_map.map_buffer = push_constants_intersect.map_buffer;

    // Rendering stage
    swapchain.renderFrameSimplified([&](imr::Swapchain::SimplifiedRenderContext
                                            &context) {
      auto &image = context.image();
      auto cmdbuf = context.cmdbuf();

      vk.cmdClearColorImage(cmdbuf, image.handle(), VK_IMAGE_LAYOUT_GENERAL,
                            tmpPtr((VkClearColorValue){
                                .float32 = {0.0f, 0.0f, 0.0f, 1.0f},
                            }),
                            1, tmpPtr(image.whole_image_subresource_range()));

      vk.cmdPipelineBarrier2(
          cmdbuf,
          tmpPtr((VkDependencyInfo){
              .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
              .dependencyFlags = 0,
              .memoryBarrierCount = 1,
              .pMemoryBarriers = tmpPtr((VkMemoryBarrier2){
                  .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                  .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                  .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
                  .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                  .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
              })}));

      if (showMap) {
        auto &map_shader = shaders->map;
        vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE,
                          map_shader.pipeline());
        // this helper class takes care of "descriptors"
        // it has to live as long as the frame rendering takes so it _cannot_ be
        // stack-allocated here instead it goes on the heap and we manually
        // delete it
        auto shader_bind_helper = map_shader.create_bind_helper();
        shader_bind_helper->set_storage_image(0, 0, image);
        shader_bind_helper->commit(cmdbuf);

        vkCmdPushConstants(cmdbuf, map_shader.layout(),
                           VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(push_constants_map), &push_constants_map);

        // We dispatch invocations in "workgroups", whose size is defined in the
        // compute shader file we need to dispatch (screenSize / workgroupSize)
        // workgroups, but rounding up if the screen size is not a multiple of
        // the workgroup size all sizes here are 3D but we use only the first
        // two to match the screen size and make the "depth" dimension just one
        vkCmdDispatch(cmdbuf, 120, 67, 1);
        context.addCleanupAction([=, &device]() { delete shader_bind_helper; });
      } else {
        auto &world_intersect_shader = shaders->world_intersect;
        vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE,
                          world_intersect_shader.pipeline());

        vkCmdPushConstants(cmdbuf, world_intersect_shader.layout(),
                           VK_SHADER_STAGE_COMPUTE_BIT, 0,
                           sizeof(push_constants_intersect),
                           &push_constants_intersect);
        vkCmdDispatch(cmdbuf, 120, 1, 1);

        vk.cmdPipelineBarrier2KHR(
            cmdbuf,
            tmpPtr((VkDependencyInfo){
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .dependencyFlags = 0,
                .memoryBarrierCount = 1,
                .pMemoryBarriers = tmpPtr((VkMemoryBarrier2){
                    .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                    .srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                    .srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT |
                                     VK_ACCESS_2_MEMORY_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                    .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT |
                                     VK_ACCESS_2_MEMORY_WRITE_BIT,
                })}));

        auto &world_render_shader = shaders->world_render;
        vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE,
                          world_render_shader.pipeline());
        // it has to live as long as the frame rendering takes so it _cannot_ be
        // stack-allocated here instead it goes on the heap and we manually
        // delete it
        auto shader_bind_helper = world_render_shader.create_bind_helper();
        shader_bind_helper->set_storage_image(0, 0, image);
        shader_bind_helper->commit(cmdbuf);

        vkCmdPushConstants(
            cmdbuf, world_render_shader.layout(), VK_SHADER_STAGE_COMPUTE_BIT,
            0, sizeof(push_constants_render), &push_constants_render);

        // We dispatch invocations in "workgroups", whose size is defined in the
        // compute shader file we need to dispatch (screenSize / workgroupSize)
        // workgroups, but rounding up if the screen size is not a multiple of
        // the workgroup size all sizes here are 3D but we use only the first
        // two to match the screen size and make the "depth" dimension just one
        vkCmdDispatch(cmdbuf, 120, 67, 1);

        context.addCleanupAction([=, &device]() { delete shader_bind_helper; });
      }
    });

    glfwPollEvents();
  }

  swapchain.drain();
  return 0;
}
