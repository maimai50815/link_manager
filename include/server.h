#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "deserialize.h"

using namespace std;

class CallbackHelper
{
public:
	CallbackHelper(){}
	
	template <typename T>
	void set(DataType type, std::function<void(const T&)> f)
	{
		switch(type){
			case DataType::Pose:{
				f1 = f;
			}break;
			case DataType::IntSingle:{
				f2 = f;
			}break;
			case DataType::IntArray:{
				f3 = f;
			}break;
		}
	}
	
	void call(const Pose& msg){ f1(msg); }
	void call(const IntSingle& msg){ f2(msg); }
	void call(const IntArray& msg){ f3(msg); }
private:
	std::function<void(const Pose&)> f1;
	std::function<void(const IntSingle&)> f2;
	std::function<void(const IntArray&)> f3;
};

class Server
{
public:
	Server();

	template <typename T>
	void determineType(T type);

	void setPort(int port){ port_ = port; }
	
	void receiveOnce();
private:
	std::shared_ptr<Deserializer> deserializer_;
	
	DataType type_;
	
	int port_;

private:
	CallbackHelper helper_;

	char recv_buf_[MESSAGE_SIZE];
};

inline Server::Server()
{
	deserializer_ = std::make_shared<Deserializer>();
}

inline void Server::receiveOnce()
{
	cout<<"receive once:"<<this->port_<<endl;
	
	memset(recv_buf_, MESSAGE_SIZE, 0);

	switch(type_)
	{
		case DataType::Pose:{
			Pose msg;
			bool ret = deserializer_->getMsg(recv_buf_, msg);
			if(!ret)
				return;
			const auto& m = msg;
			helper_.call(m);
		}break;

		case DataType::IntSingle:{
			IntSingle msg;
			bool ret = deserializer_->getMsg(recv_buf_, msg);
			if(!ret)
				return;
			const auto& m = msg;
			helper_.call(m);
		}break;

		case DataType::IntArray:{
			IntArray msg;
			bool ret = deserializer_->getMsg(recv_buf_, msg);
			if(!ret)
				return;
			const auto& m = msg;
			helper_.call(m);
		}break;
	}
}

template <typename T>
inline void Server::determineType(T type)
{
	size_t size_type = sizeof(type);
	if(size_type==sizeof(Pose))
	{
		cout<<"is Pose"<<endl;
		type_ = DataType::Pose;
	}
	else if(size_type==sizeof(IntSingle))
	{
		cout<<"is IntSingle"<<endl;
		type_ = DataType::IntSingle;
	}
	else if(size_type==sizeof(IntArray))
	{
		cout<<"is IntArray"<<endl;
		type_ = DataType::IntArray;
	}
}
#endif
