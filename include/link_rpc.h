#ifndef LINK_RPC_H
#define LINK_RPC_H

#include "common.h"

namespace link_master
{
class LinkRpc
{
public:
	LinkRpc();
	static void execute(TopicInfo&, std::vector<int>&);
private:
};

inline LinkRpc::LinkRpc()
{
}

inline void LinkRpc::execute(TopicInfo& info, std::vector<int>& ports)
{
	/* the message transformed to link master contains topic\pid\type */
	int client_sockfd;
	
	struct sockaddr_in remote_addr;
   	char recv_buf[MESSAGE_SIZE] = {0,};
   	char send_buf[MESSAGE_SIZE] = {0,};
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remote_addr.sin_port = master_port;
	
	if((client_sockfd = socket(PF_INET, SOCK_STREAM, 0))<0)
	{
		cout<<"socket error"<<endl;
		return;
	}

	struct timeval timeout = {0,1000}; 

	//setsockopt(client_sockfd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));

	//setsockopt(client_sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	
	if(connect(client_sockfd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr))<0)
	{
		cout<<"connect error"<<endl;
		return;
	}

	int intbuf[2];
	intbuf[0] = info.pid;
	intbuf[1] = info.type;
	memcpy(send_buf, intbuf, sizeof(int)*2);
	
	const char* topic_buf = info.topic.c_str();
	memcpy(send_buf+sizeof(int)*2, topic_buf, strlen(topic_buf));
	
	int send_size = send(client_sockfd, send_buf, strlen(send_buf), 0);
	if(send_size < 0)
	{
		cout<<"send to link master failed"<<endl;
		return;
	}
	
	int recv_size = recv(client_sockfd, recv_buf, MESSAGE_SIZE, 0);
	if(recv_size > 0)
	{
		cout<<"Received："<<recv_buf<<" ，Info Length："<<recv_size<<endl;
		
		int port_size = recv_size/sizeof(int);
		int port_buf[100];
		memcpy(port_buf, recv_buf, recv_size);
		ports.clear();
		for(int i = 0; i < port_size; ++i)
		{
			ports.push_back(port_buf[i]);
		}
	}
	else
	{
		cout<<"recv from link master failed"<<endl;
	}
}

}/* end of namespace */
#endif
