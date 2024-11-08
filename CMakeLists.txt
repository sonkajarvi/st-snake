cmake_minimum_required(VERSION 3.20)

project(st_snake LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(st_snake
    main.c
)

add_subdirectory(st)
target_link_libraries(st_snake PRIVATE st)

target_include_directories(st_snake PRIVATE
    st/include
    st/thirdparty/cglm/include
    st/thirdparty/glad/include
)

target_compile_options(st_snake PRIVATE
    -g
    -O2
    -Wall
    -Wextra
    -Werror
)

target_compile_definitions(st PUBLIC
    ST_CONFIG_COLORFUL_OUTPUT
)
