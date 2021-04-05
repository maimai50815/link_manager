#ifndef LINK_RPC_H
#define LINK_RPC_H

#include "common.h"

namespace link_master
{
class LinkRpc
{
public:
	LinkRpc();
	~LinkRpc();
	static void execute(TopicInfo&, std::vector<int>&, int);

	using managerFunc = std::function<void()>;
	void registCallback(managerFunc& func){ func_ = func; }
private:
	managerFunc func_;

};

inline LinkRpc::LinkRpc()
{
}

inline void LinkRpc::execute(TopicInfo& info, std::vector<int>& ports, int client_sockfd)
{
	/* the message transformed to link master contains topic\pid\type */
	
   	char recv_buf[MESSAGE_SIZE] = {0,};
   	char send_buf[MESSAGE_SIZE] = {0,};
	
	int intbuf[2];
	intbuf[0] = info.pid;
	intbuf[1] = info.type;
	
	memcpy(send_buf, intbuf, 2*sizeof(int));
	
	const char* topic_buf = info.topic.c_str();

	memcpy(send_buf+2*sizeof(int), topic_buf, 100);
	//cout<<"strlen(send_buf):"<<strlen(send_buf)<<endl;

	int send_size = send(client_sockfd, send_buf, 100, 0);
	if(send_size < 0)
	{
		cout<<"send to link master failed"<<endl;
		return;
	}
	else
	{
		//cout<<"link rpc send:"<<send_buf<<endl;
	}
	
	int recv_size = recv(client_sockfd, recv_buf, MESSAGE_SIZE, 0);
	if(recv_size > 0)
	{
		//cout<<"Received："<<recv_buf<<" ，Info Length："<<recv_size<<endl;
		
		int port_size = recv_size/sizeof(int);
		int port_buf[100];
		memcpy(port_buf, recv_buf, recv_size);
		ports.clear();
		for(int i = 0; i < port_size; ++i)
		{
			if(port_buf[i]<0)
				break;
				
			ports.push_back(port_buf[i]);
			cout<<"port_buf["<<i<<"]:"<<port_buf[i]<<endl;
		}
	}
	else
	{
		cout<<"recv from link master failed"<<endl;
	}
}

}/* end of namespace */
#endif
