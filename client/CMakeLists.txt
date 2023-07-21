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

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# For Qt
list(APPEND CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")

# We need this for the simulator, it's set to required since it should be pretty much always available
find_package(OpenGL REQUIRED)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets OpenGL Charts Multimedia PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL Charts Multimedia PrintSupport REQUIRED)

# set(PACKAGES_LIBS "${OPENGL_LIBRARIES} ${DCSMODULESAPI_LIBRARIES}")

# Check if the simulation library wrapper is installed on this system
find_package(DCSsimuWrapper QUIET)
if(DCSsimuWrapper_FOUND)
	message(STATUS "Found DCSsimuWrapper!")
	message(STATUS "Adding simulation module as a window.")
	add_definitions(-DDCSSIMU_FOUND)
	# set(PACKAGES_LIBS "${PACKAGES_LIBS} ${DCSSIMUWRAPPER_LIBRARIES}")
else()
	message(STATUS "DCSsimu was not found under the CMAKE_INSTALL_PREFIX directories.")
	message(STATUS "If you have it installed consider pointing to the install dir.")
	message(WARNING "Compiling without DCSsimu.")
endif()

# get_property(loc TARGET Qt5::Core PROPERTY IMPORTED_LOCATION_RELEASE)
# message(STATUS "${loc}")


qt5_add_resources(RESFILES res/res.qrc)

set(SOURCE
	src/common.h
	src/main.cpp
	src/windows/main_window.cpp
	src/windows/main_window.h

	src/windows/log_window.h

	src/windows/hist_window.h
	src/windows/hist_window.cpp

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

	src/windows/channelmanager_window.h
	src/windows/channelmanager_window.cpp
	src/windows/channelmanager_window.ui

	src/windows/mcaspectrum_window.h
	src/windows/mcaspectrum_window.cpp

	src/windows/layout_window.h
	src/windows/layout_window.cpp
	src/windows/layout_window.ui

	src/windows/acquisitioncontrol_window.h
	src/windows/acquisitioncontrol_window.cpp
	src/windows/acquisitioncontrol_window.ui

	src/windows/clinometer_window.h
	src/windows/clinometer_window.cpp
	src/windows/clinometer_window.ui

	src/windows/simulation_window.h
	src/windows/simulation_window.cpp
	src/windows/simulation_window.ui

	src/windows/visualizer_window.h
	src/windows/visualizer_window.cpp

	src/dialogs/connect_dialog.h
	src/dialogs/connect_dialog.ui
	
	src/dialogs/newchannel_dialog.h
	src/dialogs/newchannel_dialog.ui

	src/widgets/dnd_outputconfig.h
	src/widgets/dnd_outputconfig.ui

	src/extras/qindicator.h

	src/utils/worker.h

	src/splash/loading.h

	src/autogen_macros.h # Ui autogen friendly macros
)

# if(DEBUG_BUILD)
# 	add_executable(DCScan-Client # Enable console if in debug mode
# 		${SOURCE}
# 		${RESFILES}
# 		${app_WINRC}
# 	)
# else()
# 	add_executable(DCScan-Client WIN32 # Disable console if in release mode
# 		${SOURCE}
# 		${RESFILES}
# 		${app_WINRC}
# 	)
# endif()

add_executable(DCScan-Client
	${SOURCE}
	${RESFILES}
	${app_WINRC}
)
# Disable console if in release mode
set_target_properties(DCScan-Client PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)

target_link_libraries(DCScan-Client PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Charts
	Qt${QT_VERSION_MAJOR}::Multimedia
	Qt${QT_VERSION_MAJOR}::OpenGL
	Qt${QT_VERSION_MAJOR}::PrintSupport
	"${OPENGL_LIBRARIES}"
	"${DCSMODULESAPI_LIBRARIES}"
)

target_include_directories(DCScan-Client PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/build/include
	"${DCSMODULESAPI_INCLUDE_DIRS}"
)

if(DCSsimuWrapper_FOUND)
	message(STATUS "Linking: ${DCSSIMUWRAPPER_LIBRARIES}")
	target_link_libraries(DCScan-Client PRIVATE "${DCSSIMUWRAPPER_LIBRARIES}")
	target_include_directories(DCScan-Client PRIVATE "${DCSSIMUWRAPPER_INCLUDE_DIRS}")
	target_compile_definitions(DCScan-Client PRIVATE -D_USE_MATH_DEFINES) # Add M_PI support from <math.h>/<cmath>
endif()

# if(DEBUG_BUILD)
#     message(WARNING "Configuring client for debug mode.")

# 	# Fix linker warnings's (LNK4217) in the 3rd party library
# 	target_link_libraries(DCScan-Client PRIVATE 
# 		${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/debug/lib/qtadvanceddocking_static.lib
# 	)
# 	target_include_directories(DCScan-Client PRIVATE
# 		${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/debug/include
# 	)
# else()
# 	# Fix linker warnings's (LNK4217) in the 3rd party library
# 	target_link_libraries(DCScan-Client PRIVATE
# 	    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/lib/qtadvanceddocking_static.lib
# 	)
# 	target_include_directories(DCScan-Client PRIVATE
# 	    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking/include
# 	)
# endif()

# Fix linker warnings's (LNK4217) in the 3rd party library
target_link_libraries(DCScan-Client PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking$<$<CONFIG:Debug>:/debug>/lib/qtadvanceddocking_static.lib"
)
target_include_directories(DCScan-Client PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/qtadvanceddocking$<$<CONFIG:Debug>:/debug>/include"
)

# Copy aditional files when building
add_custom_command(TARGET DCScan-Client POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
	$<TARGET_FILE_DIR:DCScan-Client>/styles
)

# Copy default style
add_custom_command(TARGET DCScan-Client POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/styles/default.css
	$<TARGET_FILE_DIR:DCScan-Client>/styles/
)

# Copy default perspective
add_custom_command(TARGET DCScan-Client POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_CURRENT_SOURCE_DIR}/settings/ads_perspectives.ini
	$<TARGET_FILE_DIR:DCScan-Client>
)

