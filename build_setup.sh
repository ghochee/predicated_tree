#!/bin/sh

# Script which does the installation of conan packages at locations expected by
# cmake.
#
# TODO(ghochee): Call from cmake (https://github.com/conan-io/cmake-conan)

CONAN_DIRECTORY=".conan"

if [ ! -d ${CONAN_DIRECTORY} ]; then
    mkdir ${CONAN_DIRECTORY}
fi

cd ${CONAN_DIRECTORY}
conan install ..
