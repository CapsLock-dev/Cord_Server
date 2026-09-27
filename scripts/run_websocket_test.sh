#!/bin/bash

set -euo pipefail

cmake --build build/Debug -j 12 --target integration_websocket

key_path="$(pwd)/tests/integration/websocket/key.pem"
cert_path="$(pwd)/tests/integration/websocket/cert.pem"

if [[ ! -e $cert_path || ! -e $key_path ]]; then
    openssl req -x509 -newkey rsa:2048 -nodes \
    -keyout $key_path \
    -out $cert_path \
    -days 365 -subj "/CN=localhost" -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"
fi

./build/Debug/tests/integration/websocket/integration_websocket -c $cert_path -k $key_path &
SERVER_PID=$!
trap 'kill -SIGINT "$SERVER_PID" 2>/dev/null || true' EXIT

sleep 0.5

docker run --rm --network host \
    --user "$(id -u):$(id -g)" \
    -e HOME=/tmp \
    -v "$(pwd)/tests/integration/websocket/autobahn/config:/config:ro" \
    -v "$(pwd)/tests/integration/websocket/autobahn/reports:/reports" \
    crossbario/autobahn-testsuite:25.10.1 \
    wstest --spec /config/fuzzingclient.json --mode fuzzingclient

if [[ "${1:-}" == "openres" ]]; then
    xdg-open "$(pwd)/tests/integration/websocket/autobahn/reports/index.html"
fi
