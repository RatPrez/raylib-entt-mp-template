#!/usr/bin/env bash
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT/build"

CMAKE_EXTRA_ARGS=()
if [ -n "$GNS_ROOT" ]; then
    CMAKE_EXTRA_ARGS+=("-DGNS_ROOT=$GNS_ROOT")
fi

cmake -B "$BUILD_DIR" -S "$ROOT" -DCMAKE_BUILD_TYPE=Debug "${CMAKE_EXTRA_ARGS[@]}"
cmake --build "$BUILD_DIR" -- -j"$(nproc)"

ln -sf build/compile_commands.json "$ROOT/compile_commands.json"

echo ""
echo "Build complete:"
echo "  $BUILD_DIR/server/server"
echo "  $BUILD_DIR/client/client"
