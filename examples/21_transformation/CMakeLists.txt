add_executable(21_transformation 21_transformation.cpp ../common/camera.cpp)
target_link_libraries(21_transformation imr nasl::nasl)

add_custom_target(21_transformation_vert_spv COMMAND ${GLSLANG_EXE} -V -S vert ${CMAKE_CURRENT_SOURCE_DIR}/21_transformation.vert -o ${CMAKE_CURRENT_BINARY_DIR}/21_transformation.vert.spv)
add_dependencies(21_transformation 21_transformation_vert_spv)
add_custom_target(21_transformation_frag_spv COMMAND ${GLSLANG_EXE} -V -S frag ${CMAKE_CURRENT_SOURCE_DIR}/21_transformation.frag -o ${CMAKE_CURRENT_BINARY_DIR}/21_transformation.frag.spv)
add_dependencies(21_transformation 21_transformation_frag_spv)
