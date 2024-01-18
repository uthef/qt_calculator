cmake_minimum_required(VERSION 3.27)
project(qt_calculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(STREVAL_DIR /home/uthef/Documents/repos/cpp/streval)

include_directories(
        ${STREVAL_DIR}/include
        include
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(qt_calculator
        ui/calculator_form.ui
        ui/about_popup.ui
        resources.qrc
        include/Configuration.h
        include/AboutPopup.h
        include/Segment.h
        include/SegmentType.h
        include/MainWindow.h
        include/ExpressionBuilder.h
        src/main.cpp
        src/MainWindow.cpp
        src/ExpressionBuilder.cpp
        src/AboutPopup.cpp
        src/Configuration.cpp
        include/MainWindowUtils.h
)

target_link_libraries(qt_calculator
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ${STREVAL_DIR}/cmake-build-release/libstreval.a
)