cmake_minimum_required(VERSION 3.16)

project(MeasuringObserver VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Charts)

qt_standard_project_setup()

qt_add_executable(appMeasuringObserver
    main.cpp

)

qt_add_qml_module(appMeasuringObserver
    URI MeasuringObserver
    VERSION 1.0
    QML_FILES Main.qml
    QML_FILES Gauge.qml
    QML_FILES DialogChannelChange.qml
    SOURCES appcore.h appcore.cpp
)

set_target_properties(appMeasuringObserver PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMeasuringObserver
    PRIVATE Qt6::Quick
    Qt6::Charts
)

install(TARGETS appMeasuringObserver
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
