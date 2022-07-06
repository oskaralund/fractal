cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(fractal LANGUAGES CXX C)

add_executable(main src/main.cpp
                    src/shader.cpp
                    src/fractal.cpp
                    src/glad.c)
target_include_directories(main PUBLIC include)
target_link_libraries(main PUBLIC -lglfw -lGL)
target_compile_features(main PUBLIC cxx_std_20)
