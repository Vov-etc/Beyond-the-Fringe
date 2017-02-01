#ifndef NET_INCLUDES_H
#define NET_INCLUDES_H

#ifdef _WIN32
    #pragma comment ( lib, "ws2_32.lib" )
    #define _WINSOCK_DEPRECATED_NO_WARNINGS

    #include <winsock2.h>
    #include <windows.h>

    #define MSG_CONFIRM 0
    #define SHUT_RDWR 0
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #define WSAGetLastError() errno
    typedef int SOCKET;
#endif

#define SERVERADDR "127.0.0.1"
const static short PORT = 5050;

const static int MSG_FAIL = 0;
const static int MSG_OK = 1;
const static int MSG_HELLO = 2;
const static int MSG_GET_DRAW_DATA = 3;
const static int MSG_PLAYER_MOVE = 4;
const static int MSG_KEYS_DOWN = 5;


#endif // NET_INCLUDES_H
