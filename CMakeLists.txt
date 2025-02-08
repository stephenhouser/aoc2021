cmake_minimum_required(VERSION 3.25)

project(aoc2021
	VERSION 1.0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB V_GLOB LIST_DIRECTORIES true "day[012]*")
foreach(item ${V_GLOB})
  if(IS_DIRECTORY ${item})
		add_subdirectory(${item})
	endif()
endforeach()

add_executable(summary summary.cpp)

#enable_testing()
#add_subdirectory(tests)