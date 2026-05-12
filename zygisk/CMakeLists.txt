cmake_minimum_required(VERSION 3.10)
project(MockPropsModule CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mockprops SHARED module.cpp)

# Minimal linker flags for a Magisk/Zygisk module
target_link_libraries(mockprops log)
target_link_options(mockprops PRIVATE -Wl,--gc-sections -Wl,-z,hash-style=gnu -Wl,-z,relro -Wl,-z,now)