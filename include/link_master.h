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

	void deleteClosedProcess(int);
	void processServerCmd(TopicInfo&, int (&intbuf)[100]);
	void processClientCmd(TopicInfo&, int (&intbuf)[100]);

	std::unordered_map<std::string, std::vector<TopicInfo> > topic_info_map_;

	std::shared_ptr<ThreadPool> thread_pool_;
	int thread_size_ = 4;

	std::thread* main_thread_ = nullptr;

	const int master_port_ = 9000;
	int socket_fd_;

	int dispatchPort()
	{
		sockaddr_storage ss;
		socklen_t ss_len = sizeof(ss);
		getsockname(socket_fd_, (sockaddr *)&ss, &ss_len);

		sockaddr_in *sin = (sockaddr_in *)&ss;
		
		int port_new = (sin->sin_port);
		return port_new;
	}
};
}/* end of namespace */
#endif
