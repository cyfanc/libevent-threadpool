#pragma once
#include <event2/bufferevent.h>
#include <event2/event.h>

class Task
{

public:
	
	Task(struct sockaddr_in* proxy,int fd);

	~Task();

	//初始化任务
	bool init();

	void setEventBase(event_base* base){ this->base = base; }
	
	event_base* getEventBase(){return this->base;}

	void setThreadId(int threadId){ this->threadId = threadId; }

	int getThreadId(){ return this->threadId;}

	bufferevent* getClientBev(){ return this->bevClient;}

	bufferevent* getServerBev(){ return this->bevServer;}

	void clientCallBack();

	void serverCallBack();

	//初始化客户端事件
	bool initClient();

	//初始化服务端事件
	bool initServer();
	
private:

	//libevent 上下文
	event_base* base = nullptr;

	//客户端bev
	bufferevent* bevClient = nullptr;

	//服务端bev
	bufferevent* bevServer = nullptr;

	//代理ip端口信息
	struct sockaddr_in* proxy;

	//客户端事件fd
	int sockClient;
	
	//任务对应的事件线程id
	int threadId = 0;

};