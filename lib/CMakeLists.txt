set(LIBRARY_SOURCES
        "lib_echo.cpp")
set(LIBRARY_HEADERS
        "lib_echo.hpp")

add_library(${LIBRARY_NAME} STATIC
        ${LIBRARY_SOURCES}
        ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
        "./")