#include "include/net.h"
#include "include/net_includes.h"
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <thread>


void accept_new_clients(Net& net) {
    SOCKET client_sock;
    while (true) {
        net.connect_with_client(client_sock);
        net.clients[client_sock].socket = client_sock;
    }
}

int Net::get_data_timeout(SOCKET client_socket, size_t len, int sec, int usec) {
    buffer.reset();
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
    my_socket = socket(AF_INET, SOCK_STREAM, 0); // you create your socket object
    if (my_socket < 0) {
        throw std::runtime_error("Error opening socket");
    }
    sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    int i = 0;
    while (bind(my_socket, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        i++;
        serv_addr.sin_port = htons(PORT + i);
    }
    printf("port = %d\n", PORT + i);
    listen(my_socket, 5);
    std::thread accepting_thread(accept_new_clients, std::ref(*this));
    accepting_thread.detach();
}

Net::~Net() {
  #ifdef _WIN32
    WSACleanup();
  #endif
  shutdown(my_socket, SHUT_RDWR);
}

int Net::connect_with_client(SOCKET &client_socket) {
    socklen_t client_length;
    struct sockaddr_in client_addr;
    client_length = sizeof(client_addr);
    client_socket = accept(my_socket, (struct sockaddr *) &client_addr, &client_length); 
                            // at previous line you wait until one client connects with you
    if (client_socket < 0) {
        printf("ERROR on accept\n");
        return -1;
    }
    return 0;
}


void Net::new_client() {
    static int id = 0;
    buffer.read(MSG_OK);
    buffer.read(id);
    id++;
}


void Net::update_one_client(SOCKET client_socket) {
    int size = get_data_timeout(client_socket, BUFF_SIZE - 1);
    if (size <= 0) {
        return;
    }
    switch (buffer[0]) {
      case MSG_HELLO:
        printf("new client\n");
        new_client();
        break;
/*
      case MSG_GET_DRAW_DATA:
        size = world.write_bytes(buffer);
        break;

      case MSG_PLAYER_MOVE:
        size = update_processing_keys(world);
        break;
*/
      default:
        printf("No matches for message type");
        buffer.read(MSG_FAIL);
        break;
    }
    send(client_socket, buffer.get_data(), buffer.length(), MSG_CONFIRM);
}

int Net::update() {
    for (auto client : clients) {
        SOCKET client_socket = client.first;
        update_one_client(client_socket);
    }
    return 0;
}
