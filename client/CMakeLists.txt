cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The QtAds is defined for dll func, but being compiled static. This might be fixable,
# maybe the issue is on my end. Nevertheless it is not harmful.
set(CMAKE_EXE_LINKER_FLAGS "/IGNORE:4217 /NODEFAULTLIB:libcmt.lib")

# For Qt
#set(CMAKE_PREFIX_PATH "H:/Qt/5.15.1/msvc2019_64")
set(CMAKE_PREFIX_PATH "D:/Qt/5.15.1/msvc2019_64")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts Multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts Multimedia REQUIRED)

qt5_add_resources(RESFILES res/res.qrc)

set(SOURCE
	src/main.cpp
	src/windows/main_window.cpp
	src/windows/main_window.h

	src/windows/log_window.h

	src/windows/hist_window.h
	src/windows/hist_window.cpp

	src/windows/histsett_window.h
	src/windows/histsett_window.cpp

	src/windows/test_window.h
	src/windows/test_window.cpp
	src/windows/test_window.ui

	src/windows/connect_window.h
	src/windows/connect_window.cpp
	src/windows/connect_window.ui

	src/windows/freejog_window.h
	src/windows/freejog_window.cpp
	src/windows/freejog_window.ui

	src/windows/position_window.h
	src/windows/position_window.cpp
	src/windows/position_window.ui

	src/windows/channelviz_window.h
	src/windows/channelviz_window.cpp
	src/windows/channelviz_window.ui

	src/dialogs/connect_dialog.h
	src/dialogs/connect_dialog.ui

	src/extras/qindicator.h

	src/utils/worker.h

	src/splash/loading.h

	src/autogen_macros.h # Ui autogen friendly macros
)

if(DEBUG_BUILD)
	add_executable(DCScan-Client # Enable console if in debug mode
		${SOURCE}
		${RESFILES}
		${app_WINRC}
	)
else()
	add_executable(DCScan-Client WIN32 # Disable console if in release mode
		${SOURCE}
		${RESFILES}
		${app_WINRC}
	)
endif()

if(DEBUG_BUILD)
    message("Configuring client for debug mode.")

	# Fix linker warnings's (LNK4217) in the 3rd party library
	target_link_libraries(DCScan-Client PRIVATE
		Qt${QT_VERSION_MAJOR}::Widgets
		Qt${QT_VERSION_MAJOR}::Charts
		Qt${QT_VERSION_MAJOR}::Multimedia
		"${DCSMODULESAPI_LIBRARIES}"
		${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/debug/lib/qtadvanceddocking_static.lib)

	target_include_directories(DCScan-Client PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/build/include
		"${DCSMODULESAPI_INCLUDE_DIRS}"
		${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/debug/include)
else()
	# Fix linker warnings's (LNK4217) in the 3rd party library
	target_link_libraries(DCScan-Client PRIVATE
	    Qt${QT_VERSION_MAJOR}::Widgets
		Qt${QT_VERSION_MAJOR}::Charts
		Qt${QT_VERSION_MAJOR}::Multimedia
	    "${DCSMODULESAPI_LIBRARIES}"
	    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/lib/qtadvanceddocking_static.lib)

	target_include_directories(DCScan-Client PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}/build/include
	    "${DCSMODULESAPI_INCLUDE_DIRS}"
	    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/include)
endif()

