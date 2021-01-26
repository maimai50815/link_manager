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
}

inline bool Deserializer::getMsg(char buf[MESSAGE_SIZE], IntSingle& msg)
{
}

inline bool Deserializer::getMsg(char buf[MESSAGE_SIZE], IntArray& msg)
{
}
#endif
