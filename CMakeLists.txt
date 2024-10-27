cmake_minimum_required(VERSION 3.5)

project(opensync-backend LANGUAGES C)

find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(cJSON REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(mongoose STATIC external/mongoose/mongoose.c)

add_executable(opensync-backend
    main.c
    handler/handler.h
    handler/handler.c
    handler/user_handler.h
    handler/user_handler.c
    handler/file_handler.h
    handler/file_handler.c
    handler/defaults.c
    handler/defaults.h

    external/mongoose/mongoose.c
    external/mongoose/mongoose.h

    model/user.h
    model/user.c
    model/file.h
    model/file.c
    model/crypt.h
    model/crypt.c

    service/database.c
    service/database.h
    service/user_service.c
    service/user_service.h
    service/file_service.c
    service/file_service.h

    util/util.h
    util/util.c
    util/config.h
    util/config.c
)

target_link_libraries(opensync-backend ${SQLite3_LIBRARIES} ${CJSON_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${CURL_LIBRARIES})
target_include_directories(opensync-backend PRIVATE external/mongoose external/unity)

install(TARGETS opensync-backend RUNTIME DESTINATION bin)
