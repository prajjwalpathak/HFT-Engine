include_directories(${CMAKE_SOURCE_DIR}/include)

# Core library
add_library(core
    core/dummy.cpp
)

# Network library
add_library(network
    network/dummy.cpp
)

# Engine binary
add_executable(engine
    main/engine_main.cpp
)

target_link_libraries(engine PRIVATE core network)

# Gateway binary
add_executable(gateway
    main/gateway_main.cpp
)