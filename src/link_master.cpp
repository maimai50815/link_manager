#include "link_master.h"

using namespace std;

bool link_master::LinkMaster::finished_;

static void sigintHandler(int)
{
    link_master::LinkMaster::finished_ = true;
}

namespace link_master
{
LinkMaster::LinkMaster()
{
	finished_ = false;

	thread_pool_ = std::make_shared<ThreadPool>(thread_size_);
	
	initServer();

	thread_pool_->start();
	
	signal(SIGINT, sigintHandler);
	
	main_thread_ = new std::thread(&LinkMaster::mainCycle, this);
}

LinkMaster::~LinkMaster()
{
	cout<<"link manager dtor"<<endl;
	thread_pool_->stop();
}

void LinkMaster::initServer()
{
	socket_fd_ = buildSocket();
	bindSocket(master_port_, socket_fd_);
	listen(socket_fd_, 13);
	printf("Start to listen on port [%d]\n", master_port_);
}

void LinkMaster::mainCycle()
{
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	while(!finished_)
	{
		int clifd = accept(socket_fd_, (struct sockaddr *)&cliaddr, &len);
		if (clifd < 0)
		{
			printf("Accept new client failure: %s\n");
			continue;
		}
		printf("Accept new client[%s:%d] successfully\n", inet_ntoa(cliaddr.sin_addr),
				(cliaddr.sin_port));
		cout<<"clifd:"<<clifd<<endl;
		
		thread_pool_->addTask([&](){
			int fd = clifd;
			cout<<"process begin:"<<fd<<endl;
			char recv_buf[10];
			int i = 0;
			while(1)
			{
				cout<<"recv:"<<fd<<endl;
				int size = recv(fd, recv_buf, 1024, 0);

				if(size>0)
				{
					printf("%s\n", recv_buf);
					processRpcCmd();
					send(fd,to_string(fd).c_str(),21,0);
				}
				else
					break;
			}
			::close(fd);
			cout<<"process finished"<<endl;
		});
	}
	::close(socket_fd_);
	cout<<"link master main cycle finished"<<endl;
}

void LinkMaster::processRpcCmd()
{

}

}/* end of namespace */

int main(int argc, char** argv)
{
	link_master::LinkMaster lc;
	return 0;
}
