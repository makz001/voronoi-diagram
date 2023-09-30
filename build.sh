#!/usr/bin/bash

set -xe

cc -Wall -Wextra -pedantic -o voronoi voronoi.c -lm
