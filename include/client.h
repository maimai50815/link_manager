#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include "serialize.h"

template <typename T> 
class Client
{
public:
	Client();
	void publish(T& msg);
	void setPort(int port) 
	{ 
		port_ = port; 
		init_ = true;
	}
	void determineType();
	void link();
	void buildSocket();
private:
	std::shared_ptr<Serializer> serializer_;

	int port_;
	DataType type_;
	T type;

	char send_buf_[MESSAGE_SIZE];
	
	int socket_fd_;
	sockaddr_in server_addr_;

	bool socket_built_ = false;
	bool linked_ = false;

	std::string ip_ = "127.0.0.1";
private:
	bool init_ = false;
};

template <typename T> 
inline Client<T>::Client()
{
	determineType();
	serializer_ = std::make_shared<Serializer>();

	buildSocket();
}

template <typename T>
inline void Client<T>::buildSocket()
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

template <typename T>
inline void Client<T>::link()
{
	if(!socket_built_)
	{
		buildSocket();
	}
	else if(!linked_)
	{
		memset(&server_addr_,0,sizeof(server_addr_));
		server_addr_.sin_family = AF_INET;
		server_addr_.sin_port = htons(port_);

		const auto str = ip_.data();
		server_addr_.sin_addr.s_addr = inet_addr(str);

		int connect_ret = connect(socket_fd_, (sockaddr*)&server_addr_, sizeof(sockaddr));

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
}

template <typename T>
inline void Client<T>::publish(T& msg)
{
	if(!init_)
		return;
		
	link();

	if(linked_)
	{
		memset(send_buf_, 0, MESSAGE_SIZE);

		serializer_->getBuffer(msg, send_buf_);
		
		if(send(socket_fd_, send_buf_, sizeof(send_buf_), 0) < 0)
		{
			cout<<port_<<": send failed"<<endl;
		}
		else
			cout<<port_<<": send succeed"<<endl;
	}
}

template <typename T>
inline void Client<T>::determineType()
{
	cout<<"type id:"<<this->type.id<<endl;

	switch(this->type.id)
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
