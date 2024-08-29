cmake_minimum_required(VERSION 3.22)
project(pandio C)

set(CMAKE_C_STANDARD 11)
include(CTest)

include_directories(src)

set (SRC_FILES
        src/core.h
        src/internal.h
        src/tcp.h
        src/queue.h
        src/heap.h
        src/timers.h
        src/timers.c
        src/pandio.h
        src/internal.c
        src/threadpool.c
        src/threadpool.h
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND SRC_FILES
            src/sys/event_epoll.c
            src/sys/tcp_unix.c
            src/sys/thread_unix.c
    )
endif ()

set(KQUEUE_SYSTEMS "Darwin;FreeBSD;OpenBSD;NetBSD;DragonFly")

if (CMAKE_SYSTEM_NAME IN_LIST KQUEUE_SYSTEMS)
    list(APPEND SRC_FILES
            src/sys/event_kqueue.c
            src/sys/tcp_unix.c
            src/sys/thread_unix.c
    )
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    list(APPEND SRC_FILES
            src/sys/event_iocp.c
            src/sys/tcp_win32.c
            src/sys/thread_win32.c
    )
endif ()

add_library(pandio STATIC ${SRC_FILES})

add_executable(sample_server
        samples/server.c
)
target_link_libraries(sample_server PRIVATE pandio)

add_executable(sample_client
        samples/client.c
)
target_link_libraries(sample_client PRIVATE pandio)
