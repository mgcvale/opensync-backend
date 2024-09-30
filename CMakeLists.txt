cmake_minimum_required(VERSION 3.5)

project(hyprsync-backend LANGUAGES C)

find_package(PkgConfig REQUIRED)

find_package(SQLite3 REQUIRED)

pkg_check_modules(MICROHTTPD REQUIRED libmicrohttpd)

add_executable(hyprsync-backend
    main.c
    handler/handler.h
    handler/handler.c
    handler/user_handler.h
    handler/user_handler.c
    handler/defaults.c
    handler/defaults.h

    model/user.h

    service/database.c
    service/database.h
    service/user_service.c
    service/user_service.h

    util/var.h
    util/util.h
    util/util.c
)

target_include_directories(hyprsync-backend PRIVATE ${MICROHTTPD_INCLUDE_DIRS})
target_link_libraries(hyprsync-backend ${MICROHTTPD_LIBRARIES})
target_link_libraries(hyprsync-backend ${SQLite3_LIBRARIES})

install(TARGETS hyprsync-backend RUNTIME DESTINATION bin)
