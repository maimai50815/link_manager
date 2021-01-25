#ifndef SERVER_H
#define SERVER_H

#include "common.h"

using namespace std;

class Server
{
public:
	Server();

	template <typename T>
	void determineType(T type);

	void setPort(int port){ port_ = port; }
	Server* next() { return next_; }
	
	void setNext(Server* next) { next_ = next; }
	
	void receiveOnce();
private:
	
	DataType type_;
	
	int port_;

	Server* next_ = nullptr;
};

inline Server::Server()
{
}

inline void Server::receiveOnce()
{
	cout<<"receive once:"<<this->port_<<endl;
}

template <typename T>
inline void Server::determineType(T type)
{
	size_t size_type = sizeof(type);
	if(size_type==sizeof(Pose))
	{
		cout<<"is Pose"<<endl;
		type_ = DataType::Pose;
	}
	else if(size_type==sizeof(IntSingle))
	{
		cout<<"is IntSingle"<<endl;
		type_ = DataType::IntSingle;
	}
	else if(size_type==sizeof(IntArray))
	{
		cout<<"is IntArray"<<endl;
		type_ = DataType::IntArray;
	}
}
#endif
