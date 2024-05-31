#pragma once

#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>

int init_epoll();
void add_to_epoll(int, int);
void remove_from_epoll(int, int);
