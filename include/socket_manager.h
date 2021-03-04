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
	Client makeClient(std::string topic);

	template <typename T>
	Server makeServer(std::string topic, std::function<void(const T&)> f);

	void spinOnce();
private:

	std::vector<Server> server_list_;
	std::vector<Client> client_list_;
private:
	/* make sure that link-master is running */
	bool connectMaster();
	int master_fd_ = -1;
	bool master_connected_ = false;
	int retry_times_ = 0;
	const int master_port_ = 9000;
	const std::string master_ip_ = "127.0.0.1";

	int process_id_ = -1;
};

inline SocketManager::SocketManager()
{
	process_id_ = getpid();
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
inline Client SocketManager::makeClient(std::string topic)
{
	Client client;

	// if(port_map_.find(topic) != port_map_.end())
	// {
	// 	cout<<"find client match:"<<topic<<","<<port_map_[topic]<<endl;
	// 	client.setPort(port_map_[topic]);
	// }

	T type;
	client.determineType(type);

	TopicInfo info;
	info.topic = topic;
	info.pid = process_id_;
	info.type = CLIENT;
	
	client.setTopicInfo(std::move(info));
	link_master::LinkRpc::execute(info, client.getPortList());
	client_list_.push_back(client);

	return client;
}

template <typename T>
inline Server SocketManager::makeServer(std::string topic, std::function<void(const T&)> f)
{
	Server server;

	TopicInfo info;
	info.topic = topic;
	info.pid = process_id_;
	info.type = SERVER;
	server.setTopicInfo(std::move(info));

	T type;
	server.determineType(type);

	std::vector<int> ports;
	link_master::LinkRpc::execute(info, ports);
	server.setPort(ports[0]);

	server.setCallback(f);

	server_list_.push_back(server);
	
	return server;
}

inline void SocketManager::spinOnce()
{
	for(auto it = client_list_.begin(); it != client_list_.end(); ++it)
	{
		link_master::LinkRpc::execute(it->getTopicInfo(), it->getPortList());
	}

	for(auto it = server_list_.begin(); it != server_list_.end(); ++it)
	{
		std::vector<int> ports;
		link_master::LinkRpc::execute(it->getTopicInfo(), ports);
		it->setPort(ports[0]);

		//it->receiveOnce();
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
