cmake_minimum_required(VERSION 3.5)
project(milsim)

# =================================================
# GENERAL STUFF
# =================================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_BUILD_TYPE Debug)

# =================================================
# EXTERNAL LIBRARIES
# =================================================
add_subdirectory(ext)
# Logging (https://github.com/gabime/spdlog)
include_directories(ext/spdlog-0.14.0/include)
# Lua 5.3 (http://www.lua.org/)
find_package(Lua 5.3 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
# Lua bindings (https://github.com/jeremyong/Selene)
include_directories(ext/Selene-master/include)
# Argument parsing (https://github.com/Taywee/args)
include_directories(ext/args-6.2.0)
# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
# Crc32
include_directories(ext/CRCpp-master/inc)
# glm
include_directories(ext/glm-0.9.8.5)
# path handling
include_directories(ext/apathy-master)


# =================================================
# OUR STUFF
# =================================================
add_subdirectory(engine)
add_subdirectory(app)
