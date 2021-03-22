cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Qt
set(CMAKE_PREFIX_PATH "H:/Qt/5.15.1/msvc2019_64")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

add_executable(DCScan-Client #WIN32 # Disable console
	src/main.cpp
	src/windows/main_window.cpp
	src/windows/main_window.h

	src/windows/log_window.h  # The log window
	src/windows/hist_window.h # The histogram window
	src/windows/hist_window.cpp

	src/windows/histsett_window.h # The histogram settings window
	src/windows/histsett_window.cpp

	src/windows/test_window.h # Test ui files import
	src/windows/test_window.cpp # Test ui files import
	src/windows/test_window.ui # Test ui files import

	src/autogen_macros.h # Ui autogen friendly macros

	${app_WINRC}
)


# Fix linker warnings's (LNK4217) in the 3rd party library
target_link_libraries(DCScan-Client PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Charts
    "${DCSMODULESAPI_LIBRARIES}"
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/lib/qtadvanceddocking_static.lib)

target_include_directories(DCScan-Client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/build/include
    "${DCSMODULESAPI_INCLUDE_DIRS}"
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/include)
