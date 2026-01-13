cmake_minimum_required(VERSION 3.22)

project(echo_serv VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(LIBRARY_NAME lib_echo_serv)


#add_subdirectory(client/src)
add_subdirectory(client)
add_subdirectory(server)


