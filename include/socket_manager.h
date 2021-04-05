#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include "client.h"
#include "server.h"
#include "link_rpc.h"

using namespace std;

namespace socket_manager
{
bool _ok = true;

static void sigintHandler(int)
{
	cout<<"not ok"<<endl;
    _ok = false;
}

static bool ok()
{
	return _ok;
}

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
	
	void shutdown();
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

	std::thread* publish_thread_;
	void publishCycle();

	std::thread* negotiate_thread_;
	void negotiateCycle();

	std::queue<TcpTransport*> publish_queue_;

	std::mutex cycle_mutex_;
};

inline SocketManager::SocketManager()
{
	signal(SIGINT, sigintHandler);

	process_id_ = getpid();

	while(ok())
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

	publish_thread_ = new std::thread(&SocketManager::publishCycle, this);
	negotiate_thread_ = new std::thread(&SocketManager::negotiateCycle, this);
}

inline SocketManager::~SocketManager()
{
	shutdown();
	publish_thread_->join();
	delete publish_thread_;
	negotiate_thread_->join();
	delete negotiate_thread_;
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

	client.registQueue(&publish_queue_);
	
	T type;
	client.determineType(type);

	TopicInfo info;
	info.topic = topic;
	info.pid = process_id_;
	info.type = CLIENT;
	
	client.setTopicInfo(std::move(info));
	link_master::LinkRpc::execute(info, client.getPortList(), master_fd_);

	client.setId((int)client_list_.size());
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
	link_master::LinkRpc::execute(info, ports, master_fd_);
	if(!ports.empty())
		server.setPort(ports[0]);
	
	server.setCallback(f);

	server_list_.push_back(server);
	
	return server;
}

inline void SocketManager::spinOnce()
{
	for(auto it = server_list_.begin(); it != server_list_.end(); ++it)
	{
		if(it->haveClient())
			it->receiveOnce();
	}
}

inline void SocketManager::publishCycle()
{
	while(ok())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		std::lock_guard<std::mutex> lg(cycle_mutex_);

		/* get the publish queue and fill those publishers have not send messages with blank massages */
		std::vector<int> publish_list(client_list_.size(), 0);
		while(ok())
		{
			if(publish_queue_.empty())
				break;

			TcpTransport* transport = publish_queue_.front();
			publish_queue_.pop();
			
			for(int i = 0; i < (int)client_list_.size(); ++i)
			{
				if(transport->getClientId() == i)
				{
					publish_list[i] = 1;
				}
			}

			transport->send();
		}

		/* find those silent publishers and publish blank message */
		for(size_t i = 0; i < publish_list.size(); ++i)
		{
			if(publish_list[i] < 1)
			{
				client_list_[i].sendBlankMessage();
			}
		}
	}
}

inline void SocketManager::negotiateCycle()
{
	while(ok())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::lock_guard<std::mutex> lg(cycle_mutex_);

		/* negotiate with master, get the pending topics */
		for(auto it = client_list_.begin(); it != client_list_.end(); ++it)
		{
			link_master::LinkRpc::execute(it->getTopicInfo(), it->getPortList(), master_fd_);
			it->processPendingPorts();
		}

		for(auto it = server_list_.begin(); it != server_list_.end(); ++it)
		{
			std::vector<int> ports;
			link_master::LinkRpc::execute(it->getTopicInfo(), ports, master_fd_);
			if(!ports.empty())
				it->setPort(ports[0]);
				
			it->processPendingPorts();
		}
	}
}

inline void SocketManager::shutdown()
{
	TopicInfo info;
	info.pid = process_id_;
	info.type = PROCESS_CLOSE;
	info.topic == "/";
	std::vector<int> ports;
	link_master::LinkRpc::execute(info, ports, master_fd_);
}

static void spin(std::shared_ptr<SocketManager> m)
{
	if(m == nullptr)
		return;

	while(ok())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		m->spinOnce();
	}
}
}/* end of namespace */

#endif
