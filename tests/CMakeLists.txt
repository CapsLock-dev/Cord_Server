find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_subdirectory(integration)
add_subdirectory(unit)
