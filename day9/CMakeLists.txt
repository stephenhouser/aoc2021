cmake_minimum_required(VERSION 3.15)
set(CMAKE_TARGET_MESSAGES OFF)

# Extract which day this is from current directory
string(REGEX MATCH "[0-9]+$" DAY "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Configure: day${DAY}")

# Set project and target from current directory name
set(DAY_TARGET "day${DAY}")
project(${DAY_TARGET})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${DAY_TARGET} solution.cpp
	point.h point.cpp
	charmap.h charmap.cpp)

target_compile_options(${DAY_TARGET} 
	PRIVATE -O3 -Wall -Wextra -Wpedantic -Weffc++ -Wconversion -Wsign-conversion -Werror
	)

#enable_testing()
#add_test(NAME test
#		COMMAND ${DAY_TARGET} test.txt)
#set_property(TEST test
#             PROPERTY PASS_REGULAR_EXPRESSION 1)
#
#add_test(NAME input
#		COMMAND ${DAY_TARGET} input.txt)

add_custom_target(test${DAY} 
	cd ${PROJECT_SOURCE_DIR} \; ./${DAY_TARGET} test.txt
	DEPENDS ${DAY_TARGET})  

add_custom_target(live${DAY}
	cd ${PROJECT_SOURCE_DIR} \; ./${DAY_TARGET} input.txt
	DEPENDS ${DAY_TARGET})
	
add_custom_target(clean${DAY} cmake-clean)

add_custom_target(archive${DAY} 
	cd ${PROJECT_SOURCE_DIR}\; zip -e input.zip test*.txt input.txt answer.txt
)
