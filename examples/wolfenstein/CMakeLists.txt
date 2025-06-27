add_executable(wolfenstein wolfenstein.cpp)
target_link_libraries(wolfenstein imr)

add_custom_target(wolfenstein_spv COMMAND ${GLSLANG_EXE} -V -S comp ${CMAKE_CURRENT_SOURCE_DIR}/wolfenstein.glsl -o ${CMAKE_CURRENT_BINARY_DIR}/wolfenstein.spv )
add_dependencies(wolfenstein wolfenstein_spv)
