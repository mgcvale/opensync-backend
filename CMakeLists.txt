cmake_minimum_required(VERSION 3.10)
project(opensync C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/includes
        /usr/local/include
        /usr/local/share/kore/include
)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES
        "src/*.c"
        "src/handler/*.c"
        "src/model/*.c"
        "src/service/*.c"
        "src/util/*.c"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wmissing-declarations
        -Wshadow
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wpointer-arith
        -Wcast-qual
        -Wsign-compare
        -Werror
        -pedantic
        -fPIC
        -DTLS_BACKEND_OPENSSL
        -std=c11
)

target_link_libraries(${PROJECT_NAME}
        OpenSSL::SSL
        OpenSSL::Crypto
        SQLite::SQLite3
)

set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/src/test/tests.c
        ${CMAKE_SOURCE_DIR}/src/test/test_server.c
        ${CMAKE_SOURCE_DIR}/src/test/test_util.c
        PROPERTIES HEADER_FILE_ONLY TRUE
)

install(TARGETS opensync RUNTIME DESTINATION bin)