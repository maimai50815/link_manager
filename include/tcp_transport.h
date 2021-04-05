#ifndef TCP_TRANSPORT_H
#define TCP_TRANSPORT_H

#include "common.h"

using namespace std;

class TcpTransport
{
public:
    TcpTransport(){}
    TcpTransport(int);
    ~TcpTransport(){}
    void connect();
    void close();
    void send();
    int getPort(){ return port_; }
    int getClientId(){ return m_.clientId; }
    Message& getMessage(){ auto& m = m_; return m; }
private:
    int socket_fd_;
    Message m_;
    const std::string ip_ = "127.0.0.1";
    int port_;
    sockaddr_in server_addr_;
    bool linked_ = false;
};

inline TcpTransport::TcpTransport(int port) : port_(port)
{
    socket_fd_ = buildSocket();
}

inline void TcpTransport::connect()
{
    memset(&server_addr_,0,sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port_);

    const auto str = ip_.data();
    server_addr_.sin_addr.s_addr = inet_addr(str);

    int connect_ret = ::connect(socket_fd_, (sockaddr*)&server_addr_, sizeof(sockaddr));

    if(connect_ret<0)
    {
        cout<<port_<<": failed to connect !"<<endl;
        linked_ = false;
    }
    else
    {
        cout<<port_<<": connect succeed"<<endl;
        linked_ = true;
    }
}

inline void TcpTransport::close()
{
    ::close(socket_fd_);
}

inline void TcpTransport::send()
{
    if(!linked_) 
        connect();

    if(linked_)
	{
		if(::send(socket_fd_, m_.sendBuffer, strlen(m_.sendBuffer), 0) < 0)
		{
			cout<<port_<<": send failed"<<endl;
		}
		else
			cout<<port_<<": send succeed"<<endl;
	}
}
#endif