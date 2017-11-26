cmake_minimum_required(VERSION 3.5)
project(milsim_app)

include_directories(../engine/include)

set(SOURCES main.cpp)
add_executable(milsim ${SOURCES})
target_link_libraries(milsim milsim_engine)
