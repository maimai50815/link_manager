#include "socket_manager.h"

using namespace std;
using namespace std::placeholders;

namespace manager
{
class Manager
{
public:
	Manager();
	~Manager();
	std::shared_ptr<SocketManager> getSocketManager()
	{
		return socket_manager_;
	}
private:
	std::shared_ptr<SocketManager> socket_manager_;
	
	Client<Pose> pose_client_;
	Server finish_server_;
	Server data_server_;

	Pose pose_msg_;
	IntSingle finish_msg_;
	IntArray data_msg_;

	void finishCb(const IntSingle& msg);
	void dataCb(const IntArray& msg);

	std::thread* main_thread_;
	
};

inline Manager::Manager()
{
	socket_manager_ = std::make_shared<SocketManager>();

	pose_client_ = socket_manager_->makeClient<Pose>("/pose");
	finish_server_ = socket_manager_->makeServer<IntSingle>("/finish", std::bind(&Manager::finishCb, this, _1));
	data_server_ = socket_manager_->makeServer<IntArray>("/data", std::bind(&Manager::dataCb, this, _1));
	
	main_thread_ = new std::thread([&](){
		while(1)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			pose_client_.publish(pose_msg_);
		}
	});
}

inline Manager::~Manager()
{
	main_thread_->join();
	delete main_thread_;
}

inline void Manager::finishCb(const IntSingle& msg)
{
}

inline void Manager::dataCb(const IntArray& msg)
{
}
 
}/* end of namespace */

int main(int argc, char** argv)
{
	manager::Manager m;
	socket_manager::spin(m.getSocketManager());
	return 0;
}
