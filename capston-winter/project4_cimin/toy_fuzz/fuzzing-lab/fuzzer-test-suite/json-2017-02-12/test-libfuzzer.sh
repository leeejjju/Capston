#!/bin/bash
# Copyright 2017 Google Inc. All Rights Reserved.
# Licensed under the Apache License, Version 2.0 (the "License");
. $(dirname $0)/../common.sh
set -x
rm -rf $CORPUS fuzz-*.log
mkdir -p $CORPUS

#[ -e $EXECUTABLE_NAME_BASE ] && 
./test-target -artifact_prefix=$CORPUS/ -workers=$JOBS $LIBFUZZER_FLAGS $CORPUS seeds
grep "ERROR: libFuzzer: deadly signal" fuzz-0.log || exit 1
