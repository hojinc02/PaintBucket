cmake_minimum_required(VERSION 3.31.2)

project(Paint VERSION 0.1)

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}")
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warning flags
add_library(compiler_flags INTERFACE)
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>")
target_compile_features(compiler_flags INTERFACE cxx_std_17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/archive")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/library")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/runtime")

# Dynamic or static
option(BUILD_SHARED_LIBS "Build with shared/dynamic libraries" OFF)

# Main exe
add_executable(${CMAKE_PROJECT_NAME})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Add sources and libraries
add_subdirectory(src/main)
add_subdirectory(src/util)
add_subdirectory(src/paint)
add_subdirectory(src/image)
if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  add_subdirectory(src/logging)
  target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC LOGGING_ENABLED)
endif()

# Link wxWidgets
include(./link_wxWidgets.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES})

# Link compiler flags
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE compiler_flags)