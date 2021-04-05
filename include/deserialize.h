#ifndef DESERIALIZE_H
#define DESERIALIZE_H

#include "common.h"

using namespace std;

class Deserializer
{
public:
	Deserializer();
	bool getMsg(char buf[MESSAGE_SIZE], Pose& msg);
	bool getMsg(char buf[MESSAGE_SIZE], IntSingle& msg);
	bool getMsg(char buf[MESSAGE_SIZE], IntArray& msg);
private:
};

inline Deserializer::Deserializer()
{
}

inline bool Deserializer::getMsg(char buf[MESSAGE_SIZE], Pose& msg)
{
	if(buf == "blank")
	{
		cout<<"blank"<<endl;
		return false;
	}

	float pose_buf[7];
	char frame_buf[10];
 
	memcpy(pose_buf, buf, 7*sizeof(float));
	memcpy(frame_buf, buf+7*sizeof(float), 10*sizeof(char));

	msg.px = pose_buf[0];
	msg.py = pose_buf[1];
	msg.pz = pose_buf[2];
	msg.theta = pose_buf[3];
	msg.length = pose_buf[4];
	msg.width = pose_buf[5];
	msg.height = pose_buf[6];
	
	std::string str(frame_buf);
	msg.frame_id = str;

	return true;
}

inline bool Deserializer::getMsg(char buf[MESSAGE_SIZE], IntSingle& msg)
{
	if(buf == "blank")
	{
		cout<<"blank"<<endl;
		return false;
	}

	//int* intbuf = (int*)buf;
	msg.data = buf[0];

	return true;
}

inline bool Deserializer::getMsg(char buf[MESSAGE_SIZE], IntArray& msg)
{
	if(buf == "blank")
	{
		cout<<"blank"<<endl;
		return false;
	}

	msg.data.resize(5);

	int* intbuf = (int*)buf;

	for(size_t i = 0; i < msg.data.size(); ++i)
	{
		msg.data[i] = intbuf[i];
	}

	return true;
}
#endif
