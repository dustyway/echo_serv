set (LIBRARY_NAME lib_echoserv)
add_subdirectory(lib)

set(SERVER_SOURCES
        main.cpp
)

set(SERVER_NAME echo_server)

add_executable(${SERVER_NAME} ${SERVER_SOURCES})
target_link_libraries(${SERVER_NAME} PUBLIC ${LIBRARY_NAME})