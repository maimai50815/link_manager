#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "client.h"
#include "server.h"

using namespace std;

typedef std::unordered_map<std::string, int> PortMap;

class SocketManager
{
public:
	SocketManager();

	template <typename T>
	Client<T> makeClient(std::string topic);

	template <typename T>
	Server<T> makeServer(std::string topic, std::function<void(const T&)> f);

	void spinOnce();
private:
	PortMap port_map_;
};

inline SocketManager::SocketManager()
{
	port_map_.insert(make_pair("/pose", 2001));
	port_map_.insert(make_pair("/finish", 2002));
	port_map_.insert(make_pair("/data", 2003));
}

template <typename T>
inline Client<T> SocketManager::makeClient(std::string topic)
{
	Client<T> client;
	printf("port_map_:%p\n", &port_map_);

	if(port_map_.find(topic) != port_map_.end())
	{
		cout<<"match:"<<topic<<","<<port_map_[topic]<<endl;
	}

	return client;
}

template <typename T>
inline Server<T> SocketManager::makeServer(std::string topic, std::function<void(const T&)> f)
{
	Server<T> server;
	return server;
}

inline void SocketManager::spinOnce()
{
	cout<<"spinOnce"<<endl;
}

#endif
