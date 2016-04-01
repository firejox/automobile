cmake_minimum_required(VERSION 3.0)

project(AutoMobile)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Cairo REQUIRED)
find_package(GTK3 REQUIRED)


add_executable (auto_mobile 
    main.c
    application.c application.c
    canvas.c canvas.c
    common.c common.h
    scene_object.c scene_object.h
    sensor.c sensor.h
    steer_control.c steer_control.h
    vector_segment.c vector_segment.h
    world.c world.h
    point.h segment.h
    vector2d.h
    vector_macro.h
    mobile.c mobile.h
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/report.pdf
    COMMAND pandoc 
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/report.md
            -s
            -H ${CMAKE_CURRENT_SOURCE_DIR}/doc/header.tex
            -o ${CMAKE_CURRENT_SOURCE_DIR}/doc/report.pdf
    DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/doc/report.md
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/header.tex
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
    )

add_custom_target(Document ALL DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/report.pdf)


target_include_directories (auto_mobile 
    PRIVATE ${CAIRO_INCLUDE_DIRS}
    PRIVATE ${GTK3_INCLUDE_DIRS})

target_link_libraries (auto_mobile
    ${GTK3_LIBRARIES}
    ${CAIRO_LIBRARIES}
    m)
