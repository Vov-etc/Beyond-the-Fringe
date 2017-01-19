#ifndef NET_H
#define NET_H

#include "net_includes.h"
#include "buff.h"


class Net {
    SOCKET my_socket;
    buff buffer;

    int get_data_timeout(SOCKET client_socket, size_t len, int sec = 0, int usec = 0);
  public:
    Net();
    ~Net();
    void update(int msg, int x, int y);
};


#endif // NET_H
