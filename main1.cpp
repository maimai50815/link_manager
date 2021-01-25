#include "socket_manager.h"

using namespace std;
using namespace std::placeholders;

namespace manager
{
class Manager
{
public:
	Manager();
	void spin();
private:
	std::shared_ptr<SocketManager> socket_manager_;
	
	Client<Pose> pose_client_;
	Server<IntSingle> finish_server_;
	Server<IntArray> data_server_;

	void finishCb(const IntSingle& msg);
	void dataCb(const IntArray& msg);
};

inline Manager::Manager()
{
	socket_manager_ = std::make_shared<SocketManager>();

	pose_client_ = socket_manager_->makeClient<Pose>("/pose");
	finish_server_ = socket_manager_->makeServer<IntSingle>("/finish", std::bind(&Manager::finishCb, this, _1));
	data_server_ = socket_manager_->makeServer<IntArray>("/finish", std::bind(&Manager::dataCb, this, _1));
}

inline void Manager::finishCb(const IntSingle& msg)
{
}

inline void Manager::dataCb(const IntArray& msg)
{
}
 
inline void Manager::spin()
{
	while(1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		socket_manager_->spinOnce();
	}
}

}/* end of namespace */

int main(int argc, char** argv)
{
	manager::Manager m;
	m.spin();
	return 0;
}
