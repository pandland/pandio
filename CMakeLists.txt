cmake_minimum_required(VERSION 3.22)
project(pandio C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

set (SRC_FILES
        src/main.c
        src/tcp.h
        src/common.h
        src/queue.h
        src/heap.h
        src/core.h
        src/timers.h
        src/timers.c
        src/pandio.h
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    list(APPEND SRC_FILES
            src/sys/event_iocp.c
            src/sys/tcp_win32.c
    )
endif ()

add_executable(pandio ${SRC_FILES})
