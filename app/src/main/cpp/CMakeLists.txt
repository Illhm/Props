cmake_minimum_required(VERSION 3.22.1)

project("mockapi_native")

# Mock CMakeLists for building the native hook
# In a real scenario, Dobby static/shared library would be linked here.
# For this code snippet generation, we will define the library and link log/libc.

add_library(
        native-lib
        SHARED
        native-lib.cpp)

# Find Android log library
find_library(
        log-lib
        log)

# Link libraries
target_link_libraries(
        native-lib
        ${log-lib})
