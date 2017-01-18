#ifndef NET_H
#define NET_H

#include "net_includes.h"
#include <map>


class Net {
    SOCKET my_socket;
    char* buffer;

    int get_data_timeout(SOCKET client_socket, size_t len, int sec = 0, int usec = 0);
    int connect_with_client(SOCKET &client_socket);
  public:
    Net();
    ~Net();
};


#endif // NET_H
