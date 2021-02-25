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

	static bool finished_;
private:
	std::unordered_map<std::string, int> topic_port_map_;

	std::shared_ptr<ThreadPool> thread_pool_;
	int thread_size_ = 4;

};
}/* end of namespace */
#endif
