cmake_minimum_required(VERSION 3.16)

project(MeasuringObserverApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Bluetooth Charts)

qt_standard_project_setup()

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/ResIcons.rc")

qt_add_executable(appMeasuringObserverApp
    main.cpp

    ${app_icon_resource_windows}
)

qt_add_qml_module(appMeasuringObserverApp
    URI MeasuringObserverApp
    VERSION 1.0
    QML_FILES Main.qml Gauge.qml

    SOURCES     bledevice.cpp bledevice.h
                deviceinfo.cpp deviceinfo.h
    RESOURCES
            batt.png
)


set_target_properties(appMeasuringObserverApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


target_link_libraries(appMeasuringObserverApp
    PRIVATE Qt6::Quick
    Qt::Bluetooth
    Qt6::Charts
)

install(TARGETS appMeasuringObserverApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
