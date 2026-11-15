#!/bin/sh

set -e

. ./setvars.sh

owcc -std=c99 -bdos -o mines.exe mines.c
