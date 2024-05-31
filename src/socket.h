#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

void make_nonblocking(int);
int init_listener(int);
int accept_peer(int);