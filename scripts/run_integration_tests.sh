#!/bin/bash

set -euo pipefail

docker compose -f tests/integration/database/docker-compose.yml up -d
trap 'docker compose -f tests/integration/database/docker-compose.yml down -v' EXIT

cmake --build build/Debug -j 12 --target integration_database

sleep 0.5 # wait for migrations to happen

ctest --preset Debug -L integration

