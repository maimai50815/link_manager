#ifndef LINK_MASTER_H
#define LINK_MASTER_H

#include "socket_manager.h"
#include "thread_pool.h"

using namespace std;

namespace link_master
{
class LinkMaster
{
public:
	LinkMaster();
	~LinkMaster();
	void initServer();

	static bool finished_;
private:
	void mainCycle();

	void processRpcCmd(char (&recv_buf)[MESSAGE_SIZE], char (&send_buf)[MESSAGE_SIZE]);

	std::unordered_map<std::string, std::vector<TopicInfo> > topic_info_map_;

	std::shared_ptr<ThreadPool> thread_pool_;
	int thread_size_ = 4;

	std::thread* main_thread_ = nullptr;

	const int master_port_ = 9000;
	int socket_fd_;
	int port_begin_ = 1000;
};
}/* end of namespace */
#endif
