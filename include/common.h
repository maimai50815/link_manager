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

#define MESSAGE_SIZE 1024

struct IntArray
{
	std::vector<int> data;
};

struct Pose
{
	float px, py, pz;
	float theta;
	float length, width, height;
};

struct IntSingle
{
	int data;
};

struct NanValue
{
};

enum class DataType
{
	IntArray,
	Pose,
	IntSingle,
	NanValue
};

#endif
