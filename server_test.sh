#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

PORT=${PORT:-8080}

# Check that the server is running
if ! nc -z localhost $PORT; then
    echo -e "${RED}Error: No server running on port $PORT.${NC}"
    echo -e "${RED}Please run './server -p $PORT' in another terminal first.${NC}"
    exit 1
fi

MSG="Test message from client"
RESULT=$(echo "$MSG" | nc -q 1 localhost $PORT)


if [[ "$RESULT" == "$MSG" ]]; then
    echo -e "${GREEN}Test passed${NC}"
    exit 0
else
    echo -e "${RED}Test failed${NC}"
    echo "Expected: $MSG"
    echo "Got: $RESULT"
    exit 1
fi
