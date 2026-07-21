cmake_minimum_required(VERSION 3.16)

project(clusterer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

set(SOURCE_FILES
    src/main.cpp

    src/core/color_spaces/lab.h
    src/core/color_spaces/rgb.h
    src/core/color_spaces/rgba.h

    src/core/color_converter/color_converter.h src/core/color_converter/color_converter.cpp
    src/core/color_clusterer/color_clusterer.h src/core/color_clusterer/color_clusterer.cpp

    src/core/palette_color/palette_color.h

    src/logging/logger.h src/logging/logger.cpp
    src/app/app.h src/app/app.cpp
)

qt_add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(QML_FILES
    src/qml/Main.qml
)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    QML_FILES ${QML_FILES}
)

set(RESOURCE_FILES
    examples/example.jpg
    icons/app_icon.png
)

qt_target_qml_sources(${PROJECT_NAME}
    PREFIX "/"
    RESOURCES ${RESOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)
