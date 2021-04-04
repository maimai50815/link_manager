#include "socket_manager.h"

using namespace std;
using namespace std::placeholders;
using namespace socket_manager;

namespace manager
{
class Monitor
{
public:
	Monitor();
	~Monitor();
	using ManagerPtr = std::shared_ptr<SocketManager>;
	ManagerPtr getManager(){ return socket_manager_; }
private:
	ManagerPtr socket_manager_;
	
	Client pose_client_;
	Server finish_server_;
	Server data_server_;

	Pose pose_msg_;
	IntSingle finish_msg_;
	IntArray data_msg_;

	void finishCb(const IntSingle& msg);
	void dataCb(const IntArray& msg);

	std::thread* main_thread_;
};

Monitor::Monitor()
{
	socket_manager_ = std::make_shared<SocketManager>();

	pose_client_ = socket_manager_->makeClient<Pose>("/pose");
	finish_server_ = socket_manager_->makeServer<IntSingle>("/finish", std::bind(&Monitor::finishCb, this, _1));
	//data_server_ = socket_manager_->makeServer<IntArray>("/data", std::bind(&Monitor::dataCb, this, _1));
	
}

Monitor::~Monitor()
{
	cout<<"...... monitor dtor"<<endl;
	
	cout<<"...... monitor dtor finish"<<endl;
}

void Monitor::finishCb(const IntSingle& msg)
{
	cout<<"finishCb:"<<msg.data<<endl;

	pose_client_.publish(pose_msg_);

}

void Monitor::dataCb(const IntArray& msg)
{
	for(auto it = msg.data.begin(); it != msg.data.end(); ++it)
	{
		cout<<(*it)<<" ";
	}
	cout<<endl;
}
 
}/* end of namespace */

int main(int argc, char** argv)
{
	manager::Monitor m;
	socket_manager::spin(m.getManager());
	cout<<"spin finished"<<endl;
	return 0;
}
