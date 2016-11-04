project(stb)

add_library(stb_image INTERFACE)

target_include_directories(stb_image BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include)

add_library(stb::image ALIAS stb_image)
