cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DCScan-Server
	src/main.cpp
	${app_WINRC}
)


# Fix linker warnings's (LNK4217) in the 3rd party library
target_link_libraries(DCScan-Server PRIVATE
    "${DCSMODULESAPI_LIBRARIES}")

target_include_directories(DCScan-Server PRIVATE
    "${DCSMODULESAPI_INCLUDE_DIRS}")
