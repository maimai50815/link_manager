#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "deserialize.h"

using namespace std;

class CallbackHelper
{
public:
	CallbackHelper(){}
	
	void set(std::function<void(const Pose&)> f){ f1 = f; }
	void set(std::function<void(const IntSingle&)> f){ f2 = f; }
	void set(std::function<void(const IntArray&)> f){ f3 = f; }

	void call(const Pose& msg){ f1(msg); }
	void call(const IntSingle& msg){ f2(msg); }
	void call(const IntArray& msg){ f3(msg); }
private:
	std::function<void(const Pose&)> f1;
	std::function<void(const IntSingle&)> f2;
	std::function<void(const IntArray&)> f3;
};

class Server
{
public:
	Server();

	template <typename T>
	void determineType(T type);

	void setPort(int port){ port_ = port; }

	template <typename T>
	void setCallback(std::function<void(const T&)> f){ helper_.set(f); }
	
	void receiveOnce();
	void buildSocket();
	void link();
private:
	std::shared_ptr<Deserializer> deserializer_;
	
	DataType type_;
	
	int port_;

	std::string ip_ = "127.0.0.1";
	sockaddr_in local_addr_, remote_addr_;
private:
	CallbackHelper helper_;

	char recv_buf_[MESSAGE_SIZE];

	int socket_fd_, accept_fd_;
	bool socket_built_ = false;
	bool linked_ = false;
};

inline Server::Server()
{
	deserializer_ = std::make_shared<Deserializer>();
}

inline void Server::buildSocket()
{
	socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

	if(socket_fd_<0)
	{
		cerr<<"failed to create socket ... "<<endl;
		socket_built_ = false;
	}
	else
		socket_built_ = true;
}

inline void Server::link()
{
	if(!socket_built_)
    {
        buildSocket();
		return;
    }
    else if(!linked_)
    {
        int backlog = 10;
        int flag = -1;
       
		memset(&local_addr_,0,sizeof(local_addr_));

		local_addr_.sin_family = AF_INET;
		local_addr_.sin_port = htons(port_);

		local_addr_.sin_addr.s_addr = inet_addr(ip_.c_str());

		bzero(&(local_addr_.sin_zero),8);

		bool bind_ret = bind(socket_fd_, (sockaddr*)&local_addr_, sizeof(sockaddr));
		if (bind_ret == -1)
		{
			cout<<port_<<": failed to bind addr!"<<endl;
			linked_ = false;
			return;
		}
		
		bool listen_ret = listen(socket_fd_, backlog);

		if (listen_ret < 0)
		{
			cout<<port_<<": failed to listen socket!"<<endl;
			linked_ = false;
			return;
		}
		else
		{
			socklen_t addr_len = sizeof(struct sockaddr);
			accept_fd_ = accept(socket_fd_, (sockaddr*)&remote_addr_, &addr_len);
			
			cout<<port_<<": accept_fd_:"<<accept_fd_<<", accept succeed"<<endl;
			linked_ = true;
		}
    }
}

inline void Server::receiveOnce()
{
	cout<<"receive once:"<<this->port_<<endl;
	
	link();

	if(!linked_)
		return;
	
	memset(recv_buf_, 0, MESSAGE_SIZE);

    int size = recv(accept_fd_, recv_buf_, MESSAGE_SIZE, 0);

	if(size>0)
	{
		switch(type_)
		{
			case DataType::Pose:{
				Pose msg;
				bool ret = deserializer_->getMsg(recv_buf_, msg);
				if(!ret)
					return;
				const auto& m = msg;
				helper_.call(m);
			}break;

			case DataType::IntSingle:{
				IntSingle msg;
				bool ret = deserializer_->getMsg(recv_buf_, msg);
				if(!ret)
					return;
				const auto& m = msg;
				helper_.call(m);
			}break;

			case DataType::IntArray:{
				IntArray msg;
				bool ret = deserializer_->getMsg(recv_buf_, msg);
				if(!ret)
					return;
				const auto& m = msg;
				helper_.call(m);
			}break;
		}
	}
	else
		cout<<port_<<": recv empty"<<endl;
}

template <typename T>
inline void Server::determineType(T type)
{
	cout<<"type id:"<<type.id<<endl;

	switch(type.id)
	{
		case 1:{
			cout<<"is IntArray"<<endl;
			type_ = DataType::IntArray;
		}break;
		case 2:{
			cout<<"is Pose"<<endl;
			type_ = DataType::Pose;
		}break;
		case 3:{
			cout<<"is IntSingle"<<endl;
			type_ = DataType::IntSingle;
		}break;
	}
}
#endif
