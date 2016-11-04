project(obj_loader)

add_library(obj_loader INTERFACE)

target_include_directories(obj_loader BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include)
