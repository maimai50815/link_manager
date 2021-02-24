#include "socket_manager.h"
#include <signal.h>

using namespace std;
using namespace std::placeholders;

bool cycle_finished = false;
static void basicSigintHandler(int)
{
    cycle_finished = true;
    cout<<"!!!!!!!!!!!!!! basicSigintHandler"<<endl;
}

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
	std::thread* spin_thread_;
};

Manager::Manager()
{
	socket_manager_ = std::make_shared<SocketManager>();

	pose_client_ = socket_manager_->makeClient<Pose>("/pose");
	finish_server_ = socket_manager_->makeServer<IntSingle>("/finish", std::bind(&Manager::finishCb, this, _1));
	data_server_ = socket_manager_->makeServer<IntArray>("/data", std::bind(&Manager::dataCb, this, _1));
	
	signal(SIGINT, basicSigintHandler);

	main_thread_ = new std::thread([&](){
		while(!cycle_finished)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			
			pose_msg_.px += 0.1;
			pose_msg_.py -= 0.1;
			pose_msg_.pz += 0.1;
			pose_msg_.theta -= 0.1;
			pose_msg_.length += 0.1;
			pose_msg_.width -= 0.1;
			pose_msg_.height += 0.1;
			pose_msg_.frame_id = "frame_id";
			cout<<"pose_msg_.px:"<<pose_msg_.px<<endl;
		}
		cout<<"main finish"<<endl;
	});

	spin_thread_ = new std::thread([&](){
		while(!cycle_finished)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			socket_manager_->spinOnce();
		}
		cout<<"spin finish"<<endl;
	});
}

Manager::~Manager()
{
	cout<<"...... manager dtor"<<endl;
	main_thread_->join();
	spin_thread_->join();
	delete main_thread_;
	delete spin_thread_;
	cout<<"...... manager dtor finish"<<endl;
}

void Manager::finishCb(const IntSingle& msg)
{
	cout<<"finishCb:"<<msg.data<<endl;

	pose_client_.publish(pose_msg_);

}

void Manager::dataCb(const IntArray& msg)
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
	manager::Manager m;
	//socket_manager::spin(m.getSocketManager());
	
	return 0;
}
