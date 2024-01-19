#!/bin/bash

set -x
rm -rf CORPUS fuzz-*.log
mkdir -p CORPUS

echo "run fuzzer with seeds and dictionary"
export ASAN_OPTIONS=halt_on_error=1
./fuzzer_myhtml CORPUS/ SEEDS/ -dict=html.dict -workers=4
