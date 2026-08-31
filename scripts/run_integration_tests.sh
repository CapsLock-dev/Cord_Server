#!/bin/bash

set -euo pipefail

docker compose -f tests/integration/docker-compose.yml up -d
trap 'docker compose -f tests/integration/docker-compose.yml down -v' EXIT

cmake --build build/Debug -j 12

ctest --preset Debug

