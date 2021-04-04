#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include "serialize.h"
#include "tcp_transport.h"

class Client
{
public:
	Client();

	template <typename T>
	void publish(T& msg);

	void setPort(int port) 
	{
		port_ = port; 
		init_ = true;
	}

	template <typename T>
	void determineType(T type);

	void link();
	void buildSocket();
	std::vector<int>& getPortList()
	{
		auto& p = port_list_;
		return p;
	}

	void setTopicInfo(TopicInfo&& info){ topic_info_ = info; }
	TopicInfo& getTopicInfo(){ auto& p = topic_info_; return p; }
	
	using QueuePtr = std::queue<Message>*;
	void registQueue(QueuePtr ptr){ publish_queue_ptr_ = ptr; }
	void setId(int id){ id_ = id; }
	int getId(){ return id_; }
	int getSocketFd(){ return socket_fd_; }
	bool haveServer(){ return !(port_list_.empty()); }
	void processPendingPorts();
private:
	std::shared_ptr<Serializer> serializer_;

	int port_;
	DataType type_;

	char send_buf_[MESSAGE_SIZE];
	
	int socket_fd_;
	sockaddr_in server_addr_;

	bool socket_built_ = false;
	bool linked_ = false;

	std::string ip_ = "127.0.0.1";

	std::vector<int> port_list_;
private:
	bool init_ = false;

	TopicInfo topic_info_;

	QueuePtr publish_queue_ptr_;

	int id_;
private:
	std::vector<TcpTransport> transport_list_;
};

inline Client::Client()
{
	serializer_ = std::make_shared<Serializer>();

	buildSocket();
}

inline void Client::buildSocket()
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

inline void Client::link()
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
inline void Client::publish(T& msg)
{
	if(!init_)
		return;

	if(!haveServer())
		return;

	link();
	
	memset(send_buf_, 0, MESSAGE_SIZE);
	serializer_->getBuffer(msg, send_buf_);

	Message m;
	m.clientId = id_;
	memcpy(m.sendBuffer, send_buf_, MESSAGE_SIZE);
	publish_queue_ptr_->push(m);		
	

	if(linked_)
	{
		
		
		if(send(socket_fd_, send_buf_, sizeof(send_buf_), 0) < 0)
		{
			cout<<port_<<": send failed"<<endl;
		}
		else
			cout<<port_<<": send succeed"<<endl;
	}
}

template <typename T>
inline void Client::determineType(T type)
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

inline void Client::processPendingPorts()
{
	if()
		
	

}
#endif
