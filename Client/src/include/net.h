#ifndef NET_H
#define NET_H

#include "net_includes.h"
#include "buffer.h"
#include "controls.h"


class Net {
    SOCKET my_socket;
    Buffer buffer;

    int get_data_timeout(SOCKET client_socket, size_t len, int sec = 0, int usec = 0);
  public:
    Net();
    ~Net();
    void update(int msg, Controls keys);
};

#endif //NET_H
