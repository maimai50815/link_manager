#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "client.h"
#include "server.h"
#include "link_rpc.h"

using namespace std;

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

	std::vector<Server> server_list_;

private:
	/* make sure that link-master is running */
	bool connectMaster();
	int master_fd_ = -1;
	bool master_connected_ = false;
	int retry_times_ = 0;
	const int master_port_ = 9000;
	const std::string master_ip_ = "127.0.0.1";
};

inline SocketManager::SocketManager()
{
	while(1)
	{
		if(connectMaster())
		{
			master_connected_ = true;
			break;
		}

		if(retry_times_ == 0)
		{
			cout<<"Could not connect Master, retrying ..."<<endl;
		}
		++retry_times_;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

inline SocketManager::~SocketManager()
{
}

inline bool SocketManager::connectMaster()
{
	if(master_fd_<0)
		master_fd_ = buildSocket();
		
	if(master_fd_<0)
		return false;

	sockaddr_in server_addr;

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = (master_port_);

	const auto str = master_ip_.data();
	server_addr.sin_addr.s_addr = inet_addr(str);

	int connect_ret = connect(master_fd_, (sockaddr*)&server_addr, sizeof(sockaddr));

	if(connect_ret<0)
		return false;
	else
		return true;
}

template <typename T>
inline Client<T> SocketManager::makeClient(std::string topic)
{
	Client<T> client;

	// if(port_map_.find(topic) != port_map_.end())
	// {
	// 	cout<<"find client match:"<<topic<<","<<port_map_[topic]<<endl;
	// 	client.setPort(port_map_[topic]);
	// }

	TopicInfo info;

	link_master::LinkRpc::execute(info);

	return client;
}

template <typename T>
inline Server SocketManager::makeServer(std::string topic, std::function<void(const T&)> f)
{
	Server server;

	T type;
	server.determineType(type);

	// if(port_map_.find(topic) != port_map_.end())
	// {
	// 	cout<<"find server match:"<<topic<<","<<port_map_[topic]<<endl;
		
	// 	server.setPort(port_map_[topic]);
	// 	server.setCallback(f);

	// 	server_list_.push_back(server);
	// }
	
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
