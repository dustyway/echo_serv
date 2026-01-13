set(LIBRARY_SOURCES
        "EchoServer.cpp")
set(LIBRARY_HEADERS
        "EchoServer.hpp")

add_library(${LIBRARY_NAME} STATIC
        ${LIBRARY_SOURCES}
        ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})