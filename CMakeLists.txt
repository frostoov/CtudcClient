cmake_minimum_required(VERSION 3.0)
project(CtudcClient)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -pedantic")
include_directories(${PROJECT_SOURCE_DIR})
add_definitions(-DQCUSTOMPLOT_USE_LIBRARY )

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    net/ctudcconn.cpp
    ui/qconsole.cpp
    ui/qfrequency.cpp
    ui/qsettings.cpp
    ui/qstatus.cpp
    ui/qvoltage.cpp
    ui/mainwindow.cpp
    ui/qloopfreq.cpp
    ui/qcontrol.cpp
    ui/qmonitor.cpp
    ui/qchambermonitor.cpp
    ui/qchambertable.cpp
    controllers/tdccontroller.cpp
    controllers/expocontroller.cpp
    controllers/voltagecontroller.cpp
    configparser/appconfigparser.cpp
    appsettings.cpp 
    main.cpp
)

set(HEADERS
    net/ctudcconn.hpp
    ui/qconsole.hpp
    ui/qvoltage.hpp
    ui/qsettings.hpp
    ui/qcontrol.hpp
    ui/mainwindow.hpp
    ui/qfrequency.hpp
    ui/qstatus.hpp
    ui/qloopfreq.hpp
    ui/qmonitor.hpp
    ui/qchambermonitor.hpp
    ui/qchambertable.hpp
    controllers/tdccontroller.hpp
    controllers/expocontroller.hpp
    controllers/voltagecontroller.hpp
    tdc/tdc.hpp
    configparser/configparser.hpp
    configparser/appconfigparser.hpp
    appsettings.hpp
)


add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

set(Boost_USE_STATIC_LIBS ON)
find_package(
	Boost
	1.60.0
	COMPONENTS
	system
        filesystem
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
    treknet
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
