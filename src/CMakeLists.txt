
include_directories(${VULKAN_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})

add_executable(renderer main.cpp)

target_link_libraries(renderer ${VULKAN_LIBRARY} ${GLFW_LIBRARIES})
set_property(TARGET renderer PROPERTY CXX_STANDARD 11)
set_property(TARGET renderer PROPERTY CXX_STANDARD_REQUIRED ON)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/models")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/textures")

compile_shader(renderer shaders/shader.vert shaders/vert.spv)
compile_shader(renderer shaders/shader.frag shaders/frag.spv)

file(COPY models/chalet.obj DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/models")
file(COPY textures/chalet.jpg DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/textures")
