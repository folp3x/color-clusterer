cmake_minimum_required(VERSION 3.16)

project(clusterer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

set(SOURCE_FILES
    main.cpp

    # mainwindow.cpp
    # mainwindow.h
    mainwindow.ui

    core/color_spaces/lab.h
    core/color_spaces/rgb.h
    core/color_spaces/rgba.h

    core/color_converter/color_converter.h core/color_converter/color_converter.cpp
    core/color_clusterer/color_clusterer.h core/color_clusterer/color_clusterer.cpp

    logging/logger.h logging/logger.cpp
    backend/backend.h backend/backend.cpp
)

qt_add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(QML_FILES
    Main.qml
)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    QML_FILES ${QML_FILES}
)

set(RESOURCE_FILES
    examples/example.jpg
)

qt_target_qml_sources(${PROJECT_NAME}
    PREFIX "/"
    RESOURCES ${RESOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)
