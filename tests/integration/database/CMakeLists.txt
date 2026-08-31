add_executable(integration_database)

target_sources(integration_database
	PRIVATE
        test_database.cpp
)

target_link_libraries(integration_database
    PRIVATE
        database core
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests(integration_database PROPERTIES LABELS "integration")
