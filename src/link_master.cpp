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
			while(!finished_)
			{
				//cout<<"recv:"<<fd<<endl;
				int size = recv(fd, recv_buf, MESSAGE_SIZE, 0);

				if(size>0)
				{
					recv_buf[size] = '\0';
					//printf("%s\n", recv_buf);
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
	cout<<"info.pid:"<<info.pid<<endl;

	if(info.type == PROCESS_CLOSE)
	{
		/* find all clients and servers from this process id, and delete them */
		deleteClosedProcess(info.pid);
		return;
	}

	char tmp[100];
	memcpy(tmp, recv_buf+2*sizeof(int), 100);
	std::string topic(tmp);
	info.topic = topic;
	cout<<"info.topic:"<<info.topic<<endl;

	/* TODO */
	/* dispatch the ports to clients and servers */
	/* a server could accept a single client ... */
	if(info.type == SERVER)
	{
		cout<<"SERVER"<<endl;

		processServerCmd(info, send_int_buf);

		memcpy(send_buf, send_int_buf, sizeof(int));
	}
	else
	{
		cout<<"CLIENT"<<endl;

		processClientCmd(info, send_int_buf);

		memcpy(send_buf, send_int_buf, sizeof(int));
	}
}

void LinkMaster::deleteClosedProcess(int pid)
{
	cout<<"...... delete closed process:"<<pid<<endl;

	/* delete all registed infomation from this process */
	for(auto it = topic_info_map_.begin(); it != topic_info_map_.end(); ++it)
	{
		auto& vec = it->second;

		bool found = true;
		while(found)
		{
			found = false;
			for(auto vec_it = vec.begin(); vec_it != vec.end(); ++vec_it)
			{
				if(vec_it->pid == pid)
				{
					found = true;
					vec.erase(vec_it);
					break;
				}
			}
		}
	}
}

void LinkMaster::processServerCmd(TopicInfo& info, int (&send_int_buf)[100])
{
	if(topic_info_map_.find(info.topic) == topic_info_map_.end())
	{
		//cout<<"no found ........."<<endl;

		/* if a topic has never been mentioned */
		info.port = dispatchPort();
		std::vector<TopicInfo> info_list{info};
		
		topic_info_map_.insert(make_pair(info.topic, info_list));
		send_int_buf[0] = -1;
	}
	else
	{
		//cout<<"found ! ........."<<endl;

		/* if the topic has been already registered */
		auto& info_list = topic_info_map_[info.topic];
		bool find_client = false;
		for(size_t i = 0; i < info_list.size(); ++i)
		{
			if(info_list[i].type == CLIENT)
			{
				cout<<"find client port:"<<info_list[i].port<<endl;
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
			info.port = dispatchPort();
			info_list.push_back(info);
			send_int_buf[0] = -1;
		}
	}
}

void LinkMaster::processClientCmd(TopicInfo& info, int (&send_int_buf)[100])
{
	if(topic_info_map_.find(info.topic) == topic_info_map_.end())
	{
		info.port = dispatchPort();
		std::vector<TopicInfo> info_list{info};
		topic_info_map_.insert(make_pair(info.topic, info_list));
		send_int_buf[0] = -1;
	}
	else
	{
		auto& info_list = topic_info_map_[info.topic];
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

}/* end of namespace */

int main(int argc, char** argv)
{
	link_master::LinkMaster lc;
	return 0;
}
