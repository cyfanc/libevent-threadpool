#include "ThreadPool.h"
#include "Thread.h"
#include "Task.h"
#include <thread>
#include <iostream>

using namespace std;

bool ThreadPool::init(int threadCount)
{
	this->threadCount = threadCount;

	for(int i = 0; i < threadCount; i++)
	{
		Thread* thread = new Thread();

		int threadId = i + 1;
		
		thread->setThreadId(threadId);

		cout << "create thread " << threadId <<" sucess " << endl;

		//启动线程
		if(!thread->start())
		{
			cerr << "start " << threadId << " failed " << endl;
			return false;
		}

		this->threads.push_back(thread);

		this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return true;
}


void ThreadPool::dispatch(Task* task)
{
	if(!task){
		return ;
	}

	int id = (lastThreadId + 1) % threadCount;

	lastThreadId = id;

	Thread * t = this->threads[id];

	//添加线程任务
	t->addTask(task);
	
	//激活线程
	t->activate();

	return ;
}