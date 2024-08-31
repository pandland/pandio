cmake_minimum_required(VERSION 3.22)
project(pandio C)

set(CMAKE_C_STANDARD 11)
include(CTest)

include_directories(src include)

set (SRC_FILES
        include/pandio/core.h
        src/internal.h
        include/pandio/tcp.h
        src/queue.h
        src/heap.h
        include/pandio/timers.h
        src/timers.c
        src/internal.c
        src/threadpool.c
        include/pandio/threadpool.h
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND SRC_FILES
            src/sys/event_epoll.c
            src/sys/tcp_unix.c
            src/sys/notifier_unix.c
            src/sys/thread_unix.c
    )

    set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set (THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)

    add_library(pandio STATIC ${SRC_FILES})
    target_link_libraries(pandio PRIVATE pthread)
endif ()

set(KQUEUE_SYSTEMS "Darwin;FreeBSD;OpenBSD;NetBSD;DragonFly")

if (CMAKE_SYSTEM_NAME IN_LIST KQUEUE_SYSTEMS)
    list(APPEND SRC_FILES
            src/sys/event_kqueue.c
            src/sys/tcp_unix.c
            src/sys/notifier_unix.c
            src/sys/thread_unix.c
    )

    set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set (THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)

    add_library(pandio STATIC ${SRC_FILES})
    target_link_libraries(pandio PRIVATE pthread)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    list(APPEND SRC_FILES
            src/sys/event_iocp.c
            src/sys/tcp_win32.c
            src/sys/notifier_win32.c
            src/sys/thread_win32.c
    )

    add_library(pandio STATIC ${SRC_FILES})
endif ()


add_executable(sample_server
        samples/server.c
)
target_link_libraries(sample_server PRIVATE pandio)

add_executable(sample_client
        samples/client.c
)
target_link_libraries(sample_client PRIVATE pandio)
