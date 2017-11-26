cmake_minimum_required(VERSION 3.5)
project(milsim)

# =================================================
# GENERAL STUFF
# =================================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =================================================
# EXTERNAL LIBRARIES
# =================================================
add_subdirectory(ext)
# Logging (https://github.com/gabime/spdlog)
include_directories(ext/spdlog-0.14.0/include)
# Lua 5.3
find_package(Lua 5.3 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
# Lua bindings
include_directories(ext/Selene-master/include)

# =================================================
# OUR STUFF
# =================================================
add_subdirectory(engine)
add_subdirectory(app)
