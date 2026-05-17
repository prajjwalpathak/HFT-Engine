#!/bin/bash

echo "Starting exchange server..."

./build/src/exchange_server &

sleep 1

echo "Starting WebSocket bridge..."

node ws_bridge/ws_bridge.js