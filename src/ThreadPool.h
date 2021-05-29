#pragma once

#include <vector>

class Task;
class Thread;

class ThreadPool
{

public:
	//单例模式
	static ThreadPool* getInstance()
	{
		static ThreadPool pool;
		return &pool;
	}

	//初始化线程池
	bool init(int threadCount);

	//线程任务分发
	void dispatch(Task* task);

private:
	ThreadPool(){};

	//线程池数量
	int threadCount = 0;

	//线程句柄
	std::vector<Thread*> threads;

	//分发任务的线程id
	int lastThreadId = -1;
};
