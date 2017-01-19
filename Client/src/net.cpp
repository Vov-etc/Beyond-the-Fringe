#pragma comment(lib,"ws2_32.lib")
#include "include/net_includes.h"
#include "include/net.h"
#include <stdexcept>
#include <vector>
#include <thread>
#include <iostream>
#include <GL/glut.h>


using namespace std;

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
        return recv(client_socket, buffer.get_data(), len, MSG_CONFIRM);
    }
    return -1;
}

Net::Net() {
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), (WSADATA *)&buffer[0]))
    {
        throw std::runtime_error("WSAStartup error");
    }
#endif
    my_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    cout << "port = " << PORT << endl;
    if (inet_addr(SERVERADDR) != INADDR_NONE) {
        serv_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    }
    else {
        serv_addr.sin_addr.s_addr = inet_addr(gethostbyname(SERVERADDR)->h_addr_list[0]);
    }
    connect(my_socket, (sockaddr *)&serv_addr, sizeof(serv_addr));
    cerr << "Connect error: " << errno << endl;
}

Net::~Net() {
    shutdown(my_socket, SHUT_RDWR);
#ifdef _WIN32
    WSACleanup();
#endif
}

void Net::update(int msg, int x, int y) {
    int size = 0;
    switch (msg) {
    case MSG_HELLO:
        buffer[0] = msg;
        buffer[1] = 0;
        size = 1;
        send(my_socket, buffer.get_data(), size, MSG_CONFIRM);
        recv(my_socket, buffer.get_data(), BUFF_SIZE - 1, MSG_CONFIRM);
        break;

    default:
        break;
    }
    for (int i = 0; i < BUFF_SIZE && buffer[i] != 0; i++) {
        cout << (int)buffer[i] << " ";
    }
    cout << endl;
    return;
}
