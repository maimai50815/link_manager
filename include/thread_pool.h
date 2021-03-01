#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "common.h"

namespace link_master
{
class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int size);
	~ThreadPool();
	void start();
	void stop();
	void addTask(Task);
private:
	ThreadPool(const ThreadPool&) = delete;
	
	void processTask();

	std::queue<Task> task_queue_;

	std::vector<std::thread*> thread_list_;

	std::condition_variable_any cond_;
	std::mutex mutex_;

	bool started_;
	int thread_size_;
};

inline ThreadPool::ThreadPool(int size) : started_(false), thread_size_(size)
{
	thread_list_.resize(size);
}

inline ThreadPool::~ThreadPool()
{
	if(started_) stop();
}

inline void ThreadPool::start()
{
	for(auto& ptr:thread_list_)
	{
		ptr = new std::thread(&ThreadPool::processTask, this);
	}

	started_ = true;
}

inline void ThreadPool::stop()
{	
	cout<<"thread pool dtor"<<endl;
	started_ = false;

	cond_.notify_all();
	
	for(auto& ptr:thread_list_)
	{
		ptr->join();
		delete ptr;
	}
	thread_list_.clear();

	cout<<"thread pool dtor finished"<<endl;
}

inline void ThreadPool::addTask(Task task)
{
	std::unique_lock<std::mutex> lg(mutex_);
	task_queue_.push(task);
	cond_.notify_one();
}

inline void ThreadPool::processTask()
{
	cout<<"process task start"<<endl;
	while(1)
	{
		std::unique_lock<std::mutex> lg(mutex_);
	
		cond_.wait(lg, [this]{
			return (!started_ || !task_queue_.empty());
		});

		lg.unlock();
		
		if(!task_queue_.empty())
		{
			auto& task = task_queue_.front();
			task();
			task_queue_.pop();
		}

		if(!started_) break;
	}

	cout<<"process task finished"<<endl;
}

}/* end of namespace */

#endif
