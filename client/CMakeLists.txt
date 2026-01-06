set(CLIENT_SOURCES
    "main.cpp")
set(CLIENT_INCLUDES
    "./")

add_executable(${CLIENT_NAME} ${CLIENT_SOURCES})
target_include_directories(${CLIENT_NAME} PUBLIC ${CLIENT_INCLUDES})
target_link_libraries(${CLIENT_NAME} PUBLIC ${LIBRARY_NAME})