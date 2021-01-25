#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

template <typename T> 
class Client
{
public:
	Client();
	void publish(T& msg);
private:
	T type;
	int port_;
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
