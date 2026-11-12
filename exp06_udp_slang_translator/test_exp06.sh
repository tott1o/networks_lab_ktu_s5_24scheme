#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

echo "=== 1. Starting UDP Slang Server in background ==="
./server &
SERVER_PID=$!
sleep 1

cleanup() {
    echo "=== Cleaning up server (PID $SERVER_PID) ==="
    kill -9 "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
}
trap cleanup EXIT

echo "=== 2. Testing Batch Mode (Translating slang) ==="
./client "Really idc about this irl"

echo -e "\n=== 3. Testing Interactive Commands (dict, sample, exit) ==="
printf "dict\nsample\nidk tbh\nexit\n" | ./client

echo -e "\n=== Exp 06 All Tests Passed Successfully! ==="
