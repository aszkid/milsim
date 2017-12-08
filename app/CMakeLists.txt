cmake_minimum_required(VERSION 3.5)
project(milsim_app)

include_directories(../engine/include)

include_directories(./include)

set(SOURCES
	main.cpp
	states/main_screen.cpp
)
add_executable(milsim ${SOURCES})
target_link_libraries(milsim milsim_engine)

target_compile_options(milsim PRIVATE $<$<CONFIG:DEBUG>:-Wall -Wextra -g>)
target_compile_options(milsim PRIVATE $<$<CONFIG:RELEASE>:-Wall -Wextra -O3>)