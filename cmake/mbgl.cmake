if (NOT MBGL_PLATFORM)
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(MBGL_PLATFORM "macos")
    else()
        set(MBGL_PLATFORM "linux")
    endif()
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/node_modules/node-cmake/FindNodeJS.cmake)
    message(FATAL_ERROR "Can't find node-cmake")
endif()

# Load Node.js
set(NodeJS_CXX_STANDARD 14 CACHE INTERNAL "Use C++14" FORCE)
set(NodeJS_DOWNLOAD ON CACHE INTERNAL "Download node.js sources" FORCE)
set(NodeJS_USE_CLANG_STDLIB OFF CACHE BOOL "Don't use libc++ by default" FORCE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/node_modules/node-cmake)
find_package(NodeJS)

find_program(npm_EXECUTABLE
    NAMES npm
    PATHS ${NodeJS_ROOT_DIR})

if (NOT npm_EXECUTABLE)
    message(FATAL_ERROR "Could not find npm")
endif()

function(_npm_install DIRECTORY NAME ADDITIONAL_DEPS)
    SET(NPM_INSTALL_FAILED FALSE)
    if("${DIRECTORY}/package.json" IS_NEWER_THAN "${DIRECTORY}/node_modules/.${NAME}.stamp")
        message(STATUS "Running 'npm install' for ${NAME}...")
        execute_process(
            COMMAND ${NodeJS_EXECUTABLE} ${npm_EXECUTABLE} install --ignore-scripts
            WORKING_DIRECTORY "${DIRECTORY}"
            RESULT_VARIABLE NPM_INSTALL_FAILED)
        if(NOT NPM_INSTALL_FAILED)
            execute_process(COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp")
        endif()
    endif()

    add_custom_command(
        OUTPUT "${DIRECTORY}/node_modules/.${NAME}.stamp"
        COMMAND ${NodeJS_EXECUTABLE} ${npm_EXECUTABLE} install --ignore-scripts
        COMMAND ${CMAKE_COMMAND} -E touch "${DIRECTORY}/node_modules/.${NAME}.stamp"
        WORKING_DIRECTORY "${DIRECTORY}"
        DEPENDS ${ADDITIONAL_DEPS} "${DIRECTORY}/package.json"
        COMMENT "Running 'npm install' for ${NAME}...")
endfunction()

# Run submodule update
message(STATUS "Updating submodules...")
execute_process(
    COMMAND git submodule update --init mapbox-gl-js
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/mapbox-gl-js/node_modules")
    # Symlink mapbox-gl-js/node_modules so that the modules that are
    # about to be installed get cached between CI runs.
    execute_process(
         COMMAND ln -sF ../node_modules .
         WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/mapbox-gl-js")
endif()

# Add target for running submodule update during builds
add_custom_target(
    update-submodules ALL
    COMMAND git submodule update --init mapbox-gl-js
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Updating submodules..."
)

# Run npm install for both directories, and add custom commands, and a target that depends on them.
_npm_install("${CMAKE_SOURCE_DIR}" mapbox-gl-native update-submodules)
_npm_install("${CMAKE_SOURCE_DIR}/mapbox-gl-js" mapbox-gl-js "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-native.stamp")
add_custom_target(
    npm-install ALL
    DEPENDS "${CMAKE_SOURCE_DIR}/node_modules/.mapbox-gl-js.stamp"
)

# Generate source groups so the files are properly sorted in IDEs like Xcode.
function(create_source_groups target)
    get_target_property(sources ${target} SOURCES)
    foreach(file ${sources})
        get_filename_component(file "${file}" ABSOLUTE)
        string(REGEX REPLACE "^${CMAKE_SOURCE_DIR}/" "" group "${file}")
        get_filename_component(group "${group}" DIRECTORY)
        string(REPLACE "/" "\\" group "${group}")
        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

# This little macro lets you set any XCode specific property
macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro (set_xcode_property)

function(_write_xcconfig_var target var)
    get_property(result TARGET ${target} PROPERTY INTERFACE_${var} SET)
    if (result)
        get_property(result TARGET ${target} PROPERTY INTERFACE_${var})
        string(REPLACE ";" "\" \"" result "${result}")
        string(REPLACE "-" "_" target "${target}")
        file(APPEND "${CMAKE_BINARY_DIR}/config.xcconfig" "${target}_${var} = \"${result}\"\n")
    endif()
endfunction()

function(target_append_xcconfig target)
    file(APPEND "${CMAKE_BINARY_DIR}/config.xcconfig" "\n// ${target}\n")
    _write_xcconfig_var(${target} INCLUDE_DIRECTORIES)
    _write_xcconfig_var(${target} COMPILE_DEFINITIONS)
    _write_xcconfig_var(${target} COMPILE_OPTIONS)
    _write_xcconfig_var(${target} LINK_LIBRARIES)
endfunction()

# Start a new file when we're running CMake
file(WRITE "${CMAKE_BINARY_DIR}/config.xcconfig" "// Do not edit -- generated by CMake\n")

# CMake 3.1 does not have this yet.
set(CMAKE_CXX14_STANDARD_COMPILE_OPTION "-std=c++14")
set(CMAKE_CXX14_EXTENSION_COMPILE_OPTION "-std=gnu++14")
