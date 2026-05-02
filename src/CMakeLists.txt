# Core library
add_library(core
)

# Network library
add_library(network
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
