
FIND_PROGRAM (GLIB_COMPILE_RESOURCES_EXEC NAMES glib-compile-resources)


MARK_AS_ADVANCED (GLIB_COMPILE_RESOURCES_EXEC)

INCLUDE(CMakeParseArguments)

function (GLIB_COMPILE_RESOURCES VAR_NAME)
    set(options MANUAL_REGISTER BUILD_INTERNAL)
    set(onearg DEST C_NAME)
    set(multiargs SOURCE)

    cmake_parse_arguments (ARGS "${options}" "${onearg}" "${multiargs}" ${ARGN})
    
    if (NOT ARGS_DEST)
        message (fatal_error "DEST SHOULD SET")
    endif (NOT ARGS_DEST)

    if (ARGS_MANUAL_REGISTER)
        set(gresource_flag "--manual-register")
    endif (ARGS_MANUAL_REGISTER)

    if (ARGS_BUILD_INTERNAL)
        set(gresource_flag "${gresource_flag} --internal")
    endif (ARGS_BUILD_INTERNAL)

    if (ARGS_C_NAME)
        set(gresource_flag "${gresource_flag} --c-name=${ARGS_C_NAME}")
    endif (ARGS_C_NAME)

    set(${VAR_NAME}_HEADER "${ARGS_DEST}.h")
    set(${VAR_NAME}_SOURCE "${ARGS_DEST}.c")
    set(${VAR_NAME}_RESOURCE ${${VAR_NAME}_SOURCE} ${${VAR_NAME}_HEADER})

    message (STATUS "header ${${VAR_NAME}_HEADER}")
    message (STATUS "source ${${VAR_NAME}_SOURCE}")

    message ("cur_dir: ${CMAKE_CURRENT_SOURCE_DIR}")
    
    add_custom_command (
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${${VAR_NAME}_SOURCE}
        COMMAND 
            ${GLIB_COMPILE_RESOURCES_EXEC}
            "--generate-source"
            "--target=${${VAR_NAME}_SOURCE}"
            ${gresource_flag}
            ${ARGS_SOURCE}
        MAIN_DEPENDENCY
            ${ARGS_SOURCE}

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "build resource ${ARGS_SOURCE}"
    )

    add_custom_command (
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${${VAR_NAME}_HEADER}
        COMMAND 
            ${GLIB_COMPILE_RESOURCES_EXEC}
            "--generate-header"
            "--target=${${VAR_NAME}_HEADER}"
            ${gresource_flag}
            ${ARGS_SOURCE}
        MAIN_DEPENDENCY
            ${ARGS_SOURCE}

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "build resource ${ARGS_SOURCE}"
    )

    set(${VAR_NAME}_HEADER PARENT_SCOPE)
    set(${VAR_NAME}_SOURCE PARENT_SCOPE)

    set(${VAR_NAME}_RESOURCE 
        ${${VAR_NAME}_SOURCE} ${${VAR_NAME}_HEADER} PARENT_SCOPE)

endfunction (GLIB_COMPILE_RESOURCES)

