#!/usr/bin/bash

gcc -std=c89 -fsanitize=address -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -D_GNU_SOURCE server.c -o server
