


set(SOURCE_FILES
  application.cpp
  buffer.cpp
  commandbuffer.cpp
  device.cpp
  image.cpp
  instance.cpp
  main.cpp
  model.cpp
  resource.cpp
  shader.cpp
  swapchain.cpp
  texture.cpp
  utils.cpp)

add_executable(renderer ${SOURCE_FILES})

target_include_directories(renderer PUBLIC ${VULKAN_INCLUDE_DIR} PUBLIC ${GLFW_INCLUDE_DIR})

target_link_libraries(renderer
  ${VULKAN_LIBRARY}
  ${GLFW_LIBRARIES}
  obj_loader
  stb::image
  dynlink)

set_property(TARGET renderer PROPERTY CXX_STANDARD 11)
set_property(TARGET renderer PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS renderer EXPORT renderer RUNTIME DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/..")

compile_shader(renderer "${CMAKE_CURRENT_SOURCE_DIR}/../shaders/shader.vert"
                        "${CMAKE_CURRENT_BINARY_DIR}/../shaders/vert.spv")
compile_shader(renderer "${CMAKE_CURRENT_SOURCE_DIR}/../shaders/shader.frag"
                        "${CMAKE_CURRENT_BINARY_DIR}/../shaders/frag.spv")
