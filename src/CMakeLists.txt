
set(SOURCE_FILES
  application.cpp
  buffer.cpp
  commandbuffer.cpp
  device.cpp
  image.cpp
  instance.cpp
  main.cpp
  model.cpp
  pipeline.cpp
  renderpass.cpp
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

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Creating Debug Build!")
  target_compile_definitions(renderer PUBLIC DEBUG_BUILD)
else()
  message(STATUS "Creating Normal Build!")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

set_property(TARGET renderer PROPERTY CXX_STANDARD 11)
set_property(TARGET renderer PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS renderer EXPORT renderer RUNTIME DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/..")

compile_shader(renderer "${CMAKE_CURRENT_SOURCE_DIR}/../shaders/shader.vert"
                        "${CMAKE_CURRENT_BINARY_DIR}/../shaders/vert.spv")
compile_shader(renderer "${CMAKE_CURRENT_SOURCE_DIR}/../shaders/shader.frag"
                        "${CMAKE_CURRENT_BINARY_DIR}/../shaders/frag.spv")
