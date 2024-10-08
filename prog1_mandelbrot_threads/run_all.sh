#!/bin/bash

# Iterate from 2 to 8
for loop_idx in {2..8}; do
  # Execute program a with command line argument '-t loop_idx'
  echo "number of threads: $loop_idx"
  ./mandelbrot -t "$loop_idx"
done
