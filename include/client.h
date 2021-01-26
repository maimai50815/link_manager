#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include "serialize.h"

template <typename T> 
class Client
{
public:
	Client();
	void publish(T& msg);
private:
	std::shared_ptr<Serializer> serializer_;
};

template <typename T> 
Client<T>::Client()
{
}

template <typename T>
inline void Client<T>::publish(T& msg)
{
}

#endif
