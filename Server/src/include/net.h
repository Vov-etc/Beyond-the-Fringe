#ifndef NET_H
#define NET_H

#include "net_includes.h"
#include "buffer.h"
#include <map>

struct client {
    int ID;
    SOCKET socket;
    client(int id = -1, int sock = -1):
        ID(id),
        socket(sock)
        {}
};

class Net {
    SOCKET my_socket;
    Buffer buffer;
    std::map<SOCKET, client> clients;

    void update_one_client(SOCKET client_socket);
    int get_data_timeout(SOCKET client_socket, size_t len, int sec = 0, int usec = 0);
    int update_processing_keys();
    int connect_with_client(SOCKET &client_socket);
    void new_client();
    int get_data_timeout(SOCKET client_socket, char* buff, size_t len, int sec = 0, int usec = 10000);
  public:
    Net();
    ~Net();
    int update();
    friend void accept_new_clients(Net& net);
};


#endif // NET_H
