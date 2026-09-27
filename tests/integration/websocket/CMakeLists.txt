add_executable(integration_websocket)

target_sources(integration_websocket
	PRIVATE
        test_websocket.cpp
)

target_link_libraries(integration_websocket
    PRIVATE
        core network
        Boost::program_options
)

