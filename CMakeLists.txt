cmake_minimum_required(VERSION 3.16.3)
project(process VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
	main.cpp
)
set(HEADERS
	process.hpp
	ipc/pipe.hpp
)

add_executable(${PROJECT_NAME}
	${SOURCES}
	${HEADERS}
)

