# Set C++98 standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common source files
set(COMMON_SOURCES
        DiagnosticsClient.cpp
        DiagnosticsServer.cpp
        Main.cpp
        TcpServer.cpp
)

# Build with poll() reactor
add_executable(tcpserv_poll
        ${COMMON_SOURCES}
        PollReactor.cpp
)
target_include_directories(tcpserv_poll PUBLIC .)
target_link_libraries(tcpserv_poll PUBLIC ${LIBRARY_NAME})

# Build with select() reactor
add_executable(tcpserv_select
        ${COMMON_SOURCES}
        SelectReactor.cpp
)
target_include_directories(tcpserv_select PUBLIC .)
target_link_libraries(tcpserv_select PUBLIC ${LIBRARY_NAME})
target_compile_definitions(tcpserv_select PRIVATE USE_SELECT_REACTOR)