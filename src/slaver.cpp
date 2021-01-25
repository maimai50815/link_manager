#include "socket_manager.h"

using namespace std;
using namespace std::placeholders;

namespace manager
{
class Slaver
{
public:
	Slaver();
	~Slaver();
	std::shared_ptr<SocketManager> getSocketManager()
	{
		return socket_manager_;
	}
private:
	std::shared_ptr<SocketManager> socket_manager_;

	Server pose_server_;
	Client<IntSingle> finish_client_;
	Client<IntArray> data_client_;

	void poseCb(const Pose& msg);
	
	std::thread* main_thread_;
};

inline Slaver::Slaver()
{
	socket_manager_ = std::make_shared<SocketManager>();
	
	finish_client_ = socket_manager_->makeClient<IntSingle>("/finish");
	data_client_ = socket_manager_->makeClient<IntArray>("/data");
	pose_server_ = socket_manager_->makeServer<Pose>("/pose", std::bind(&Slaver::poseCb, this, _1));
}

inline Slaver::~Slaver()
{
	main_thread_->join();
	delete main_thread_;
}

inline void Slaver::poseCb(const Pose& msg)
{
}

}/* end of namespace */

int main(int argc, char** argv)
{
	manager::Slaver s;
	socket_manager::spin(s.getSocketManager());
	return 0;
}
