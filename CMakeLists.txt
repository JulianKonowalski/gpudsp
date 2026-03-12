cmake_minimum_required(VERSION 3.8)
project(gpudsp LANGUAGES CXX)

#------------------------------------------------------------------------------#

find_package(OpenGL REQUIRED)

#------------------------------------------------------------------------------#

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE DEBUG)

#------------------------------------------------------------------------------#

set(GLAD_DIR "${CMAKE_SOURCE_DIR}/3rdparty/glad")
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/3rdparty/glfw")
set(SALAD_DIR "${CMAKE_SOURCE_DIR}/3rdparty/salad")

add_subdirectory(${GLAD_DIR})
add_subdirectory(${GLFW_DIR})
add_subdirectory(${SALAD_DIR})

#------------------------------------------------------------------------------#

add_executable(${PROJECT_NAME})

file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} 
    PRIVATE opengl32
    PRIVATE glad
    PRIVATE glfw
    PRIVATE salad
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE SHADER_PATH="${CMAKE_SOURCE_DIR}/src/dsp/"
    PRIVATE ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/"
)

#------------------------------------------------------------------------------#