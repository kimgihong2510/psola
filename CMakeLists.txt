cmake_minimum_required(VERSION 3.22)
project(psola VERSION 1.0.0)

option(ENABLE_PSOLA_EXAMPLE "Enable building psola example" ON)

add_subdirectory(dep/pyin-pitch-detection)
add_subdirectory(src)

if(ENABLE_PSOLA_EXAMPLE)
    add_subdirectory(example)
endif()


