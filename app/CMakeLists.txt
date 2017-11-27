cmake_minimum_required(VERSION 3.5)
project(milsim_app)

include_directories(../engine/include)
include_directories(../ext/glad/4.5-core/include)

include_directories(./include)

set(SOURCES
	main.cpp
	states/main_screen.cpp

	# opengl loader
	../ext/glad/4.5-core/src/glad.c
)
add_executable(milsim ${SOURCES})
target_link_libraries(milsim milsim_engine)
