cmake_minimum_required(VERSION 3.5)

add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(glbinding)
include_directories(glbinding/source/glbinding/include/)

add_subdirectory(tinyobjloader-1.0.6)
include_directories(ext/tinyobjloader-1.0.6)