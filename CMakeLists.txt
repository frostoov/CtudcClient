cmake_minimum_required(VERSION 3.0)
project(CtudcClient)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic")
include_directories(${PROJECT_SOURCE_DIR})

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    net/request.cpp
    net/response.cpp
    net/ctudcconn.cpp
    ui/qconsole.cpp
    ui/qfrequency.cpp
    ui/qsettings.cpp
    ui/qcontrol.cpp
    ui/qstatus.cpp
    ui/qvoltage.cpp
    ui/mainwindow.cpp
    ui/qloopfreq.cpp
    controllers/expocontroller.cpp
    controllers/tdccontroller.cpp
    controllers/voltagecontroller.cpp
    utils.cpp
    configparser/appconfigparser.cpp
    main.cpp
)

set(HEADERS
    net/request.hpp
    net/response.hpp
    net/ctudcconn.hpp
    ui/qconsole.hpp
    ui/qvoltage.hpp
    ui/qsettings.hpp
    ui/qcontrol.hpp
    ui/mainwindow.hpp
    ui/qfrequency.hpp
    ui/qstatus.hpp
    ui/qloopfreq.hpp
    controllers/tdccontroller.hpp
    controllers/voltagecontroller.hpp
    controllers/expocontroller.hpp
    tdc/tdc.hpp
    utils.hpp
    configparser/configparser.hpp
    configparser/appconfigparser.hpp
)


add_executable(${PROJECT_NAME} ${SOURCES})

set(Boost_USE_STATIC_LIBS ON)
find_package(
		Boost
		1.59.0
		COMPONENTS
		system
		REQUIRED
)
include_directories(${Boost_INCLUDE_DIRS})

# Widgets finds its own dependencies.
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Widgets REQUIRED)
qt5_use_modules(${PROJECT_NAME} Core Gui Widgets PrintSupport)

target_link_libraries(
    ${PROJECT_NAME}
    pthread
    ${Boost_LIBRARIES}
)

if(UNIX)
    target_link_libraries(
	${PROJECT_NAME}
	qcustomplot
    )
endif()
if(WIN32)
    target_link_libraries(
	${PROJECT_NAME}
	qcustomplot1
	ws2_32
	wsock32
    )
endif()
