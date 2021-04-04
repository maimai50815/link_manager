#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "common.h"

class TcpTransport
{
public:
    TcpTransport();
    void connect();
private:
    int socket_fd_;
};

inline TcpTransport::TcpTransport()
{

}
#endif