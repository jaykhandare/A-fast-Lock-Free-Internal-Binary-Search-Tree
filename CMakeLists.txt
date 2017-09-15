cmake_minimum_required(VERSION 3.7)
project(project_final)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

set(SOURCE_FILES main.cpp ds_libraries.h flag_functions.h function_declarations.h bst_functions.h other_functions.h)
add_executable(project_final ${SOURCE_FILES} )
