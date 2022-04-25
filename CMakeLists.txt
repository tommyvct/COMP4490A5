cmake_minimum_required(VERSION 3.21)
project(COMP4490A5)

set(CMAKE_CXX_STANDARD 11)

option(OPENMP "Use OpenMP acceleration" ON)

# Dependencies
include_directories("./glm")

if (WIN32)
    add_compile_definitions(FREEGLUT_STATIC)
    add_compile_definitions(FREEGLUT_LIB_PRAGMAS=0)
    set(GLUT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/freeglut/include/")
    set(GLUT_glut_LIBRARY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/freeglut/lib/freeglut_staticd.lib")
    set(GLUT_glut_LIBRARY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/freeglut/lib/freeglut_static.lib")

    add_compile_definitions(GLEW_STATIC)
    set(GLEW_USE_STATIC_LIBS TRUE)
    set(GLEW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glew/include/")
    set(GLEW_STATIC_LIBRARY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/glew32s.lib")
    set(GLEW_STATIC_LIBRARY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/glew32s.lib")
    set(GLEW_SHARED_LIBRARY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/glew32.lib")
    set(GLEW_SHARED_LIBRARY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/glew32.lib")
elseif (APPLE)
    link_libraries("-framework OpenGL")

    add_compile_definitions(GLEW_STATIC)
    set(GLEW_USE_STATIC_LIBS TRUE)
    set(GLEW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glew/include/")
    set(GLEW_STATIC_LIBRARY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/libGLEW.a")
    set(GLEW_STATIC_LIBRARY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/glew/lib/libGLEW.a")

    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLUT_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

link_libraries(${OPENGL_opengl_LIBRARY})
link_libraries(${GLUT_LIBRARIES})
link_libraries(${GLEW_LIBRARIES})

# Targets
add_executable(Assignment5
    src/main.cpp
    src/q1.cpp
    src/raytracer.cpp
    src/json2scene.cpp)

if(OPENMP)
    find_package(OpenMP)

    if(OpenMP_CXX_FOUND)
        target_link_libraries(Assignment5 PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

# Debugging
if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Assignment5)
    set_property(TARGET Assignment5
        PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
elseif (CMAKE_GENERATOR STREQUAL Xcode)
    set_property(TARGET Assignment5 
        PROPERTY XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
else()
    message(NOTICE "Please make sure the working directory is set to ${CMAKE_CURRENT_SOURCE_DIR} before debugging and running.")
endif()
