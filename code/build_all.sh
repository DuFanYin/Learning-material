#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
CODE_DIR="$ROOT_DIR/code"
BUILD_DIR="$CODE_DIR/build"

mkdir -p "$BUILD_DIR"

echo "==> Building all .cpp files under code/"
for file in "$CODE_DIR"/*/*.cpp "$CODE_DIR"/*/*/*.cpp; do
  if [[ ! -f "$file" ]]; then
    continue
  fi
  if [[ "$file" == "$CODE_DIR/build/"* ]]; then
    continue
  fi
  rel="${file#$CODE_DIR/}"
  name="${rel%.cpp}"
  safe_name="${name//\//_}"
  out="$BUILD_DIR/$safe_name"
  std="-std=c++23"

  flags="-O2"
  if [[ "$file" == *"/infra/"* ]] || [[ "$file" == *"/perf/"* ]] || [[ "$file" == *"thread"* ]] || [[ "$file" == *"spsc"* ]]; then
    flags="$flags -pthread"
  fi

  echo "  [$std] $safe_name"
  clang++ "$std" $flags "$file" -o "$out"
done

echo "==> Done. Binaries are in: $BUILD_DIR"
