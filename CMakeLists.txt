cmake_minimum_required(VERSION 3.0)

project(hyprsync-backend LANGUAGES C)

add_executable(hyprsync-backend main.c)

install(TARGETS hyprsync-backend RUNTIME DESTINATION bin)
