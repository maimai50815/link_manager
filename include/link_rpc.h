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
	const int master_port_ = 9000;
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
	remote_addr.sin_port = master_port_;
	
	if((client_sockfd=socket(PF_INET, SOCK_STREAM, 0))<0)
	{
		cout<<"socket error"<<endl;
		return;
	}

	struct timeval timeout = {0,1000}; 
	//设置发送超时
	setsockopt(client_sockfd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));

	//设置接收超时
	setsockopt(client_sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	
	//绑定服务器网络地址
	if(connect(client_sockfd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr))<0)
	{
		cout<<"connect error"<<endl;
		return;
	}

	int intbuf[2];
	intbuf[0] = info.pid;
	intbuf[1] = info.type;
	memcpy(send_buf, intbuf, sizeof(int)*2);

	int size = send(client_sockfd, send_buf, strlen(send_buf), 0);
}

}/* end of namespace */
#endif
