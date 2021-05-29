#pragma once
#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class ThreadPool;

class Server
{
public:
	
	Server(const char* ip, int port,const char* proxyIp, int proxyPort);

	~Server();

	//初始化服务器
	bool init();

	//连接事件处理
	void accept(evutil_socket_t fd);

	//运行事件循环
	void run();

private:
	//监听的ip端口信息
	struct sockaddr_in server;

	//代理的ip端口信息
	struct sockaddr_in proxy;

	//event_base
	struct event_base* base = NULL;

	//线程池信息
	ThreadPool* pool = NULL;
};