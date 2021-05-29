#include "Thread.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <event2/event.h>
#include "Task.h"

using namespace std;

Thread::Thread()
{

}

Thread::~Thread()
{

}

//启动线程
bool Thread::start()
{
	if(!init()){
		return false;
	}

	thread th(&Thread::threadFun,this);
	
	th.detach();

	return true;
}

static void notifyCallBack(evutil_socket_t fd,short which,void* arg)
{
	Thread* t = (Thread*)arg;

	t->notify(fd,which);

}

void Thread::notify(evutil_socket_t fd,short which)
{
	char buf[2] = {0};

	int ret = read(fd,buf,1);

	if(ret <= 0){
		return ;
	}

	printf("\033[32m thread %d recv new task\033[0m\n",getThreadId());

	//获取任务
	tasksMutex.lock();

	if(tasks.empty()){
		tasksMutex.unlock();
		return ;
	}

	//取出任务
	Task* task = NULL;
	task = tasks.front();
	tasks.pop_front();
	tasksMutex.unlock();

	//执行任务
	if(!task->init()){

		delete task;
		
	}
}

bool Thread::init()
{
	
	//初始化管道用于激活线程 0读取 1写入
	int fds[2];
	if(pipe(fds)){
		cerr << "pipe failed " << endl;
		return false;
	}

	//用于发送激活事件
	notifySendFd = fds[1];

	//创建libevent
	event_config* ev_conf = event_config_new();
	
	event_config_set_flag(ev_conf,EVENT_BASE_FLAG_NOLOCK);
	
	this->base = event_base_new_with_config(ev_conf);
	
	event_config_free(ev_conf);

	if(!base){
		cerr << "event_base_new_with_config failed " << endl;
		return false;
	}

	//添加激活事件监听
	event* ev = event_new(base,fds[0],EV_READ|EV_PERSIST,notifyCallBack,this);
	
	event_add(ev,0);

	return true;
}


void Thread::addTask(Task* task)
{

	task->setEventBase(getEventBase());

	task->setThreadId(getThreadId());

	tasksMutex.lock();
	
	this->tasks.push_back(task);
	
	tasksMutex.unlock();

	return ;
}


void Thread::activate()
{
	
	int ret = write(this->notifySendFd,"c",1);

	if(ret <= 0){
		cerr << "activate failed " << endl;
	}

	return ;
}

//线程函数入口
void Thread::threadFun()
{
	
	cout  << "thread " << getThreadId() << " begin " << endl;
	
	//事件循环
    event_base_dispatch(base);

    //资源释放
	event_base_free(base);
    
	cout  << "thread " << getThreadId() << " end " << endl;
}
