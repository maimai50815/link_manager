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

	std::vector<int>& getPortList()
	{
		auto& p = port_list_;
		return p;
	}

	void setTopicInfo(TopicInfo&& info){ topic_info_ = info; }
	TopicInfo& getTopicInfo(){ auto& p = topic_info_; return p; }
	
	using QueuePtr = std::queue<TcpTransport*>*;
	void registQueue(QueuePtr ptr){ publish_queue_ptr_ = ptr; }
	void setId(int id){ id_ = id; }
	int getId(){ return id_; }
	int getSocketFd(){ return socket_fd_; }
	bool haveServer(){ return !(port_list_.empty()); }
	void processPendingPorts();

	void sendBlankMessage();
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
	std::vector<TcpTransport*> transport_list_;
};

inline Client::Client()
{
	serializer_ = std::make_shared<Serializer>();
}

template <typename T>
inline void Client::publish(T& msg)
{
	if(!init_)
		return;

	if(!haveServer())
		return;

	memset(send_buf_, 0, MESSAGE_SIZE);
	serializer_->getBuffer(msg, send_buf_);
	
	for(auto transport:transport_list_)
	{
		auto& m = transport->getMessage();
		m.clientId = id_;
		memcpy(m.sendBuffer, send_buf_, MESSAGE_SIZE);
		publish_queue_ptr_->push(transport);		
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
	/* compare the current port list and new negotiated list */
	for(auto& port:port_list_)
	{
		bool found = false;
		for(auto& transport:transport_list_)
		{
			if(port == transport->getPort())
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			/* add new transports */
			transport_list_.push_back(new TcpTransport(port));
		}
	}

	for(auto it = transport_list_.begin(); it != transport_list_.end(); ++it)
	{
		bool found = false;
		for(auto& port:port_list_)
		{
			if(port == (*it)->getPort())
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			/* delete the outline links */
			(*it)->close();
			delete (*it);
			transport_list_.erase(it);
			if(it == transport_list_.end())
				break;
		}
	}
}

inline void Client::sendBlankMessage()
{
	//cout<<transport_list_.size()<<endl;
	for(auto& transport:transport_list_)
	{
		auto& m = transport->getMessage();
		char buf[MESSAGE_SIZE] = "blank";
		memcpy(m.sendBuffer, buf, MESSAGE_SIZE);
		transport->send();
	}
}
#endif
