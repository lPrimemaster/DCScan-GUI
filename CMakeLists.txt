cmake_minimum_required(VERSION 3.5)

project(DCScan-GUI LANGUAGES CXX)

# Common API for client and server
find_package(DCSModulesAPI CONFIG REQUIRED)

if(DCSModulesAPI_FOUND)
    message(STATUS "Found DCSModulesAPI!")
	message(STATUS "Lib include location: ${DCSMODULESAPI_INCLUDE_DIRS}")
	message(STATUS "Lib bin     location: ${DCSMODULESAPI_LIBRARIES}")
endif(DCSModulesAPI_FOUND)

set(app_WINRC src/app_win32.rc)
set_property(SOURCE app_WINRC APPEND PROPERTY
        OBJECT_DEPENDS ${PROJECT_SOURCE_DIR}/res/icons/icon.ico
        )

add_subdirectory(client)
add_subdirectory(server)
