add_executable(wolfenstein wolfenstein.cpp)
target_link_libraries(wolfenstein imr)

add_custom_target(map_spv COMMAND ${GLSLANG_EXE} -V -S comp ${CMAKE_CURRENT_SOURCE_DIR}/map.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/map.spv )
add_dependencies(wolfenstein map_spv)

add_custom_target(world_intersect_spv COMMAND ${GLSLANG_EXE} -V -S comp ${CMAKE_CURRENT_SOURCE_DIR}/world_intersect.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/world_intersect.spv )
add_dependencies(wolfenstein world_intersect_spv)

add_custom_target(world_render_spv COMMAND ${GLSLANG_EXE} -V -S comp ${CMAKE_CURRENT_SOURCE_DIR}/world_render.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/world_render.spv )
add_dependencies(wolfenstein world_render_spv)
