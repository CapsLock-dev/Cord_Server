# CordServer

# Building 

## REQUIREMENTS:
- CMAKE VER >= 3.25
- C++ STANDARD >= 23
- Docker
- vcpkg (VCPKG_ROOT should be set)

1. Run `cmake --list-presets` to see available presets
2. Run `cmake --preset <preset-name>` to generate build files
3. Run `cmake --build --preset <preset-name>` to build project

# Running integration tests

## Database

Database integration tests use their own database container, docker-compose is located in tests/integration/database
Always run them with script `./scripts/run_integration_tests.sh`
