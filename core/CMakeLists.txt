cmake_minimum_required(VERSION 3.20)

#------------------------------------------------------------------------------#

project(core LANGUAGES C)

if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project can only be compiled with Emscripten.")
endif()

#------------------------------------------------------------------------------#

set(WEBGPU_PATH ${CMAKE_CURRENT_LIST_DIR}/3rdparty/webgpu)
set(EMSDK_PATH ${CMAKE_CURRENT_LIST_DIR}/3rdparty/emsdk)
set(TARGET_PATH ${CMAKE_CURRENT_LIST_DIR}/../frontend/core)

#------------------------------------------------------------------------------#

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CMAKE_C_STANDARD 99
    CMAKE_C_EXTENSIONS OFF
    CMAKE_C_STANDARD_REQUIRED ON
    CMAKE_EXPORT_COMPILE_COMMANDS ON
    SUFFIX ".js"
)

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.c)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} 
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE ${EMSDK_PATH}/upstream/emscripten/cache/sysroot/include
    PRIVATE ${WEBGPU_PATH}
)

#------------------------------------------------------------------------------#

target_link_options(${PROJECT_NAME} PRIVATE
    -sWASM=1
    -sASYNCIFY=1
    -sFULL_ES3=1
    -sMODULARIZE=1
    -sEXPORT_ES6=1
    -sALLOW_MEMORY_GROWTH=1
    -sALLOW_TABLE_GROWTH
    -sENVIRONMENT=web
    -sEXIT_RUNTIME=0
    -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','addFunction','HEAPF32']
    "--preload-file" "${CMAKE_SOURCE_DIR}/shaders@/shaders"
)

#------------------------------------------------------------------------------#

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.js
        ${TARGET_PATH}/${PROJECT_NAME}.js

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.wasm
        ${TARGET_PATH}/${PROJECT_NAME}.wasm

    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.data
        ${TARGET_PATH}/${PROJECT_NAME}.data
)

#------------------------------------------------------------------------------#