#!/bin/bash

# This script takes the first argument provided to this script and sets that as
# an environment variable. Then it invokes the rest of the arguments in a
# subprocess.
#
# This inane script is required because:
# - cmake can produce arguments for commands but cannot set environment
#   variables.
# - sphinx-build has no way of accepting a path to set except through an
#   environment variable that we read as normal python.
# - doxyrest has a sphinx module which needs to be available in the conf.py
#   that we call sphinx-build with.

export DOXYREST_ROOT=$1
shift 1

$@
