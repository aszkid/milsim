cmake_minimum_required(VERSION 3.5)

# build and pack external libs ad libitum
add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(glbinding)
include_directories(glbinding/source/glbinding/include/)