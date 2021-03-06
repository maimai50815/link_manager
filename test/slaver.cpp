#include "socket_manager.h"
#include <signal.h>

using namespace std;
using namespace std::placeholders;
using namespace socket_manager;

namespace manager
{
class Slaver
{
public:
	Slaver();
	~Slaver();
	using ManagerPtr = std::shared_ptr<SocketManager>;
	ManagerPtr getManager(){ return socket_manager_; }
private:
	ManagerPtr socket_manager_;

	Server pose_server_;
	Client finish_client_;
	Client data_client_;

	IntSingle finish_msg_;
	IntArray data_msg_;

	void poseCb(const Pose& msg);
	
	std::thread* main_thread_, *spin_thread_;
};

Slaver::Slaver()
{
	socket_manager_ = std::make_shared<SocketManager>();
	
	finish_client_ = socket_manager_->makeClient<IntSingle>("/finish");
	//data_client_ = socket_manager_->makeClient<IntArray>("/data");
	pose_server_ = socket_manager_->makeServer<Pose>("/pose", std::bind(&Slaver::poseCb, this, _1));

	data_msg_.data.resize(5);
	
	main_thread_ = new std::thread([&](){
		while(socket_manager::ok())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			//finish_client_.publish(finish_msg_);
			//data_client_.publish(data_msg_);

			finish_msg_.data += 1;

			data_msg_.data[0] += 1;
			data_msg_.data[1] -= 1;
			data_msg_.data[2] += 1;
			data_msg_.data[3] -= 1;
			data_msg_.data[4] += 1;
		}
	});

}

Slaver::~Slaver()
{
	cout<<"...... slaver dtor"<<endl;
	main_thread_->join();
	delete main_thread_;
	cout<<"...... slaver dtor finish"<<endl;
}

void Slaver::poseCb(const Pose& msg)
{
	cout<<"receive pose:"<<msg.px<<","<<msg.py<<","<<msg.pz<<","<<msg.theta<<\
		","<<msg.length<<","<<msg.width<<","<<msg.height<<endl;
	cout<<"frame_id:"<<msg.frame_id<<endl;
}

}/* end of namespace */

int main(int argc, char** argv)
{
	manager::Slaver s;
	socket_manager::spin(s.getManager());
	return 0;
}
