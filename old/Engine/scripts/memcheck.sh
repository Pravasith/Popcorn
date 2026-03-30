#!/bin/bash

src_dir="$PWD"

valgrind --leak-check=yes "$src_dir/build/linux/Game"
