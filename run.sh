#!/bin/bash
cd "$(dirname "$0")"
cmake --build build -- -j8
build/FootballManagerGame
