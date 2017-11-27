cmake_minimum_required(VERSION 3.5)
project(milsim_engine)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(include)

set(SOURCES
	src/core.cpp
	
	src/sys/hermes.cpp
	src/sys/script.cpp
	src/sys/audio.cpp
	src/sys/graph.cpp
	src/sys/net.cpp
)
add_library(milsim_engine STATIC ${SOURCES})
target_link_libraries(milsim_engine ${LUA_LIBRARY})
target_link_libraries(milsim_engine Threads::Threads)
target_link_libraries(milsim_engine ${OPENGL_gl_LIBRARY})
target_link_libraries(milsim_engine glfw ${GLFW_LIBRARIES})
