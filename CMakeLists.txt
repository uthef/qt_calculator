cmake_minimum_required(VERSION 3.27)
project(qt_calculator)
add_subdirectory(expeval)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
        expeval/include
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
        include/Formatting.h
        include/ExpressionBuilder.h
        src/main.cpp
        src/MainWindow.cpp
        src/Formatting.cpp
        src/Segment.cpp
        src/ExpEvalScope.cpp
        src/ExpressionBuilder.cpp
        src/AboutPopup.cpp
        src/Configuration.cpp
)

target_link_libraries(qt_calculator
        Qt::Core
        Qt::Gui
        Qt::Widgets
        expeval
)
