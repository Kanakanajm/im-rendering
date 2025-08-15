add_executable(wolfenstein wolfenstein.cpp)
target_link_libraries(wolfenstein imr)

add_custom_target(world_spv COMMAND ${GLSLANG_EXE} -V -S comp ${CMAKE_CURRENT_SOURCE_DIR}/world.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/world.spv )
add_dependencies(wolfenstein world_spv)
