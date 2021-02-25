#include "link_master.h"

using namespace std;

bool link_master::LinkMaster::finished_;

static void basicSigintHandler(int)
{
    link_master::LinkMaster::finished_ = true;
}

namespace link_master
{
LinkMaster::LinkMaster()
{
	finished_ = false;

	thread_pool_ = std::make_shared<ThreadPool>(thread_size_);
	
	thread_pool_->start();
	
	signal(SIGINT, basicSigintHandler);
	
	for(int i = 0; i < 20; ++i)
	{
		cout<<"add task"<<endl;
		thread_pool_->addTask([i](){
			cout<<"i:"<<i<<endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		});
	}

	while(!finished_)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

LinkMaster::~LinkMaster()
{
	cout<<"link manager dtor"<<endl;
	thread_pool_->stop();
}

}/* end of namespace */

int main(int argc, char** argv)
{
	link_master::LinkMaster lc;
	return 0;
}
