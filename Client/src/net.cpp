#pragma comment(lib,"ws2_32.lib")
#include "include/net_includes.h"
#include "include/net.h"

#include <cstdio>   
#include <stdexcept>
#include <vector>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif


int Net::get_data_timeout(SOCKET client_socket, size_t len, int sec, int usec) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(client_socket, &readfds);
    struct timeval timeout;
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    if (select(client_socket + 1, &readfds, NULL, NULL, &timeout) > 0) {
        return recv(client_socket, buffer, len, MSG_CONFIRM);
    }
    return -1;
}

Net::Net() {
    buffer = new char[BUFF_SIZE];
  #ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), (WSADATA *)&buffer[0]))
    {
        throw std::runtime_error("WSAStartup error");
    }
  #endif
    my_socket = socket(AF_INET, SOCK_STREAM, 0); // you create your socket object
    sockaddr_in serv_addr;
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    printf("port = %d\n", PORT); 
    if (inet_addr(SERVERADDR) != INADDR_NONE) {
        serv_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    } else {
        serv_addr.sin_addr.s_addr = inet_addr(gethostbyname(SERVERADDR)->h_addr_list[0]);
    }
    if (connect(my_socket, (sockaddr *)&serv_addr, sizeof(serv_addr))) {
        printf("Connect error %d", errno);
    }
}

Net::~Net() {
  delete[] buffer;
  shutdown(my_socket, SHUT_RDWR);
  #ifdef _WIN32
    WSACleanup();
  #endif
}

int Net::connect_with_client(SOCKET &client_socket) {
    socklen_t client_length;
    struct sockaddr_in client_addr;
    client_length = sizeof(client_addr);
    client_socket = accept(my_socket, (struct sockaddr *) &client_addr, &client_length); 
    // at previous line you wait until one client connects with you
    return 0;
}
