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

	std::unordered_map<std::string, int> topic_port_map_;

	std::shared_ptr<ThreadPool> thread_pool_;
	int thread_size_ = 4;

	std::thread* main_thread_;

	const int master_port_ = 9000;
};
}/* end of namespace */
#endif
