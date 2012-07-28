#!/bin/bash

# This script sets up the project in debug mode
# This must be run from the build folder!!!
# Simply run make afterwards

cmake -DCMAKE_BUILD_TYPE=Debug ..
