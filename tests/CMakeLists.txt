project(Tests C)

set(CMAKE_C_STANDARD 99)

set(SRC ${CMAKE_SOURCE_DIR}/src/scanner.c
        ${CMAKE_SOURCE_DIR}/src/scanner.h)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(Tests main.c tests.c tests.h ${SRC})
