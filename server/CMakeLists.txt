set(SERVER_SOURCES
        EchoServer.cpp
        main.cpp
)
set(SERVER_INCLUDES
        "./")

add_executable(${SERVER_NAME} ${SERVER_SOURCES})
target_include_directories(${SERVER_NAME} PUBLIC ${SERVER_INCLUDES})
target_link_libraries(${SERVER_NAME} PUBLIC ${LIBRARY_NAME})