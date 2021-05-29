#pragma once
#include <list>
#include <mutex>
#include <event2/event.h>

class Task;

class Thread
{

public:
	Thread();
	~Thread();

	//启动线程
	bool start();

	//线程函数入口
	void threadFun();

	//初始化event_base
	bool init();

	//激活线程
	void activate();

	//任务通知处理函数
	void notify(evutil_socket_t fd,short which);

	//添加任务
	void addTask(Task* task);

	//获取线程id
	int getThreadId(){return threadId;}

	//设置线程Id
	void setThreadId(int threadId){ this->threadId = threadId;}

	//获取event_base
	struct event_base* getEventBase(){ return this->base; }

private:

	//线程ID
	int threadId = 0;

	//用于发送激活线程事件
	int notifySendFd = 0;

	//event_base
	struct event_base* base = 0;

	//任务链表
	std::list<Task*> tasks;

	//线程锁
	std::mutex tasksMutex;
};
