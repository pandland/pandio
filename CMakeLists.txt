cmake_minimum_required(VERSION 3.26)
project(pandio C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(pandio
        src/main.c
        src/tcp.h
        src/common.h
        src/queue.h
        src/heap.h
        src/sys/event_iocp.c
        src/core.h
        src/timers.h
        src/timers.c)
