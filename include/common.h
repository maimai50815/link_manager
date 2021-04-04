#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <thread>
#include <fstream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>
#include <functional>
#include <iterator>
#include <queue>

#include <sys/socket.h>//1connect
#include <sys/types.h>//2connect
#include <netinet/in.h> //struct
#include <string.h>//memset
#include <stdio.h>//gets
#include <unistd.h>//close
#include <arpa/inet.h>//inet

#include <signal.h>

#define MESSAGE_SIZE 128

using namespace std;

#define CLIENT 1
#define SERVER 2
#define PROCESS_CLOSE -1

static int master_port = 9000;

struct TopicInfo
{
	int pid = -1;
	int type = -1; 
	int port = -1;
	std::string topic;
};

/* datatype */
struct IntArray
{
	int id = 1;
	std::vector<int> data;
};

struct Pose
{
	int id = 2;
	float px, py, pz;
	float theta;
	float length, width, height;
	std::string frame_id;
};

struct IntSingle
{
	int id = 3;
	int data;
};

struct NanValue
{
	int id = 0;
};

enum class DataType
{
	IntArray,
	Pose,
	IntSingle,
	NanValue
};

struct Message
{
	bool isNan = false;
	int socketFd;
	int clientId = -1;
	char sendBuffer[MESSAGE_SIZE];
};

static int buildSocket()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	return fd;
}

static int bindSocket(int port, int socket_fd)
{
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = (port);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); /* 监听本机所有IP*/
	//inet_aton("192.168.0.16", &servaddr.sin_addr); /* 监听指定ip */

	int rv = bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (rv < 0)
	{
		printf("Socket[%d] bind on port[%d] failure\n", socket_fd, port);
		return -1;
	}
	return 1;
}

#endif
