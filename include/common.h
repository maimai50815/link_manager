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

#include <sys/socket.h>//1connect
#include <sys/types.h>//2connect
#include <netinet/in.h> //struct
#include <string.h>//memset
#include <stdio.h>//gets
#include <unistd.h>//close
#include <arpa/inet.h>//inet

#define MESSAGE_SIZE 1024

using namespace std;

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

#endif
