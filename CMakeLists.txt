cmake_minimum_required(VERSION 3.16)
project(PolygonChart VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Controls)

qt_add_executable(polygon-chart
    src/main.cpp
    src/RadarChartModel.h src/RadarChartModel.cpp
    src/RadarChartRenderer.h src/RadarChartRenderer.cpp
)

qt_add_qml_module(polygon-chart
    URI PolygonChart
    VERSION 1.0
    QML_FILES qml/main.qml
)

set_target_properties(polygon-chart PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(polygon-chart PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2)

qt_android_generate_deployment_settings(polygon-chart)