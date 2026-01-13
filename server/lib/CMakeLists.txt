set(LIBRARY_SOURCES
        Acceptor.cpp
        ConnectionHandler.cpp
        Reactor.cpp
        SignalHandler.cpp
        Server.cpp
)

add_library(${LIBRARY_NAME} STATIC
        ${LIBRARY_SOURCES}
)

target_include_directories(${LIBRARY_NAME} PUBLIC include)
