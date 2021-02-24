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
	~SocketManager();

	template <typename T>
	Client<T> makeClient(std::string topic);

	template <typename T>
	Server makeServer(std::string topic, std::function<void(const T&)> f);

	void spinOnce();
private:
	PortMap port_map_;

	std::vector<Server> server_list_;
};

inline SocketManager::SocketManager()
{
	port_map_.insert(make_pair("/pose", 2001));
	port_map_.insert(make_pair("/finish", 2002));
	port_map_.insert(make_pair("/data", 2003));
	port_map_.insert(make_pair("/layout", 2004));
	port_map_.insert(make_pair("/triggered", 2005));
	port_map_.insert(make_pair("/cache", 2006));
	port_map_.insert(make_pair("/switch", 2007));
	port_map_.insert(make_pair("/fake", 2008));
	port_map_.insert(make_pair("/clear", 2009));
}

inline SocketManager::~SocketManager()
{
}

template <typename T>
inline Client<T> SocketManager::makeClient(std::string topic)
{
	Client<T> client;

	if(port_map_.find(topic) != port_map_.end())
	{
		cout<<"find client match:"<<topic<<","<<port_map_[topic]<<endl;
		client.setPort(port_map_[topic]);
	}

	return client;
}

template <typename T>
inline Server SocketManager::makeServer(std::string topic, std::function<void(const T&)> f)
{
	Server server;

	T type;
	server.determineType(type);

	if(port_map_.find(topic) != port_map_.end())
	{
		cout<<"find server match:"<<topic<<","<<port_map_[topic]<<endl;
		
		server.setPort(port_map_[topic]);
		server.setCallback(f);

		server_list_.push_back(server);
	}
	
	return server;
}

inline void SocketManager::spinOnce()
{
	for(auto it = server_list_.begin(); it != server_list_.end(); ++it)
	{
		it->receiveOnce();
	}
}

namespace socket_manager
{
static void spin(std::shared_ptr<SocketManager> m)
{
	if(m == nullptr)
		return;

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		m->spinOnce();
	}
}
}/* end of namespace */

#endif
