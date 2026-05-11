include_directories(${CMAKE_SOURCE_DIR}/include)

# Core library
add_library(core
    core/dummy.cpp
)

# Engine binary
add_executable(engine
    main/engine_main.cpp
    memory/order_pool.cpp
    network/udp_publisher.cpp
)

# Exchange Server
add_executable(exchange_server
    network/tcp_server.cpp
    memory/order_pool.cpp
    network/udp_publisher.cpp
)

# Test Client
add_executable(test_client
    network/test_client.cpp
)

# Gateway binary
add_executable(gateway
    main/gateway_main.cpp
)

target_link_libraries(engine PRIVATE core)

target_include_directories(exchange_server
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(test_client
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
