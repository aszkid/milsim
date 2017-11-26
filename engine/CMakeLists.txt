cmake_minimum_required(VERSION 3.5)
project(milsim_engine)

include_directories(include)

set(SOURCES
	src/core.cpp
	src/sys/script.cpp
	src/sys/audio.cpp
	src/sys/comm.cpp
	src/sys/graph.cpp
	src/sys/net.cpp
)
add_library(milsim_engine STATIC ${SOURCES})
