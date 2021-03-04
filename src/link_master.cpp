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

	if(main_thread_)
	{
		main_thread_->join();
		delete main_thread_;
	}

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
			char recv_buf[MESSAGE_SIZE];
			char send_buf[MESSAGE_SIZE];
			int i = 0;
			while(1)
			{
				cout<<"recv:"<<fd<<endl;
				int size = recv(fd, recv_buf, MESSAGE_SIZE, 0);

				if(size>0)
				{
					recv_buf[size] = '\0';
					printf("%s\n", recv_buf);
					processRpcCmd(recv_buf, send_buf);
					send(fd, send_buf, strlen(send_buf), 0);
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

void LinkMaster::processRpcCmd(char (&recv_buf)[MESSAGE_SIZE], char (&send_buf)[MESSAGE_SIZE])
{
	int recv_int_buf[100];
	int send_int_buf[100];
	memcpy(recv_int_buf, recv_buf, 2*sizeof(int));

	TopicInfo info;
	info.pid = recv_int_buf[0];
	info.type = recv_int_buf[1];
	
	char tmp[100];
	memcpy(tmp, recv_buf+2*sizeof(int), strlen(recv_buf)-2*sizeof(int));
	std::string topic(tmp);
	info.topic = topic;

	/* TODO */
	/* dispatch the ports to clients and servers */
	/* a server could accept a single client ... */
	if(info.type == SERVER)
	{
		if(topic_info_map_.find(topic) == topic_info_map_.end())
		{
			/* if a topic has never been mentioned */
			info.port = port_dispatch_;
			++port_dispatch_;
			std::vector<TopicInfo> info_list{info};
			
			topic_info_map_.insert(make_pair(topic, info_list));
			send_int_buf[0] = -1;
		}
		else
		{
			/* if the topic has been already registered */
			auto& info_list = topic_info_map_[topic];
			bool find_client = false;
			for(size_t i = 0; i < info_list.size(); ++i)
			{
				if(info_list[i].type == CLIENT)
				{
					send_int_buf[0] = info_list[i].port;
					info.port = info_list[i].port;
					info_list.push_back(info);
					find_client = true;
					break;
				}
			}

			/* if there is no client yet, store this server */
			if(!find_client)
			{
				info.port = port_dispatch_;
				++port_dispatch_;
				info_list.push_back(info);
				send_int_buf[0] = -1;
			}
		}

		memcpy(send_buf, send_int_buf, sizeof(int));
	}
	else
	{
		if(topic_info_map_.find(topic) == topic_info_map_.end())
		{
			info.port = port_dispatch_;
			++port_dispatch_;
			std::vector<TopicInfo> info_list{info};
			topic_info_map_.insert(make_pair(topic, info_list));
			send_int_buf[0] = -1;
			memcpy(send_buf, send_int_buf, sizeof(int));
		}
		else
		{
			auto& info_list = topic_info_map_[topic];
			int ind = 0;
			for(size_t i = 0; i < info_list.size(); ++i)
			{
				if(info_list[i].type == SERVER)
				{
					send_int_buf[ind] = info_list[i].port;
					++ind;
				}
			}
		}
	}
}

}/* end of namespace */

int main(int argc, char** argv)
{
	link_master::LinkMaster lc;
	return 0;
}
