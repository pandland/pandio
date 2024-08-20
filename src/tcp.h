#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>
typedef SOCKET pd_socket_t;

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int pd_socket_t;

#endif

/* struct that represents TCP connection and stream */
struct pd_tcp_s {
    pd_socket_t fd;
};

typedef struct pd_tcp_s pd_tcp_t;
