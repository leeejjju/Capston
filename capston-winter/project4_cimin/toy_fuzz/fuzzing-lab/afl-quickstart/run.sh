#!/bin/bash
afl-gcc -o vulnerable vulnerable.c
afl-fuzz -i inputs -o outputs ./vulnerable
