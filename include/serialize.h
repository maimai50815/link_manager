#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "common.h"

class Serializer
{
public:
	Serializer();
	void getBuffer(Pose& msg, char send_buf[MESSAGE_SIZE]);
	void getBuffer(IntSingle& msg, char send_buf[MESSAGE_SIZE]);
	void getBuffer(IntArray& msg, char send_buf[MESSAGE_SIZE]);
private:
};

inline Serializer::Serializer()
{
}

inline void Serializer::getBuffer(Pose& msg, char send_buf[MESSAGE_SIZE])
{
	float pose_buf[7];
	pose_buf[0] = msg.px;
	pose_buf[1] = msg.py;
	pose_buf[2] = msg.pz;
	pose_buf[3] = msg.theta;
	pose_buf[4] = msg.length;
	pose_buf[5] = msg.width;
	pose_buf[6] = msg.height;

	memcpy(send_buf, pose_buf, 7*sizeof(float));
}

inline void Serializer::getBuffer(IntSingle& msg, char send_buf[MESSAGE_SIZE])
{
	int intbuf[1] = {0};
	intbuf[0] = msg.data;

	char* char_buf = (char*)(&intbuf);
	memcpy(send_buf, char_buf, 1*sizeof(char));
}

inline void Serializer::getBuffer(IntArray& msg, char send_buf[MESSAGE_SIZE])
{
	int intbuf[msg.data.size()];
	std::copy(msg.data.begin(), msg.data.end(), intbuf);
	memcpy(send_buf, intbuf, msg.data.size()*sizeof(int));
}
#endif
