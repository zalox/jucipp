#!/bin/bash

if [ "${make_command}" == "" ]; then
  make_command="make -j 2"
fi

cd jucipp/build || exit

if [ "${CC}" != "clang" ]; then
    exec sh -c "scan-build -o ../html_${distribution} --status-bugs ${make_command}"
fi
