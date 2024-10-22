cmake_minimum_required(VERSION 3.5)

project(opensync-test LANGUAGES C)

find_package(CURL REQUIRED)
add_library(unity STATIC ../external/unity/unity.c)

add_executable(opensync-test
    test_server.c
    tests.c
    tests.h
    util.c
    util.h

    ../external/unity/unity.h
    ../external/unity/unity_internals.h
    ../external/unity/unity.c
)


target_link_libraries(opensync-test ${CURL_LIBRARIES})
target_include_directories(opensync-test PRIVATE ../external/unity)
