#include "ThreadPool.h"
#include "Task.h"
#include "Server.h"
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

static void listenCallBack(struct evconnlistener* listener,evutil_socket_t fd,struct sockaddr *address,int socklen,void *arg)
{

    Server* server = (Server*)arg;

    server->accept(fd);
    
    return;
}

void Server::accept(evutil_socket_t fd)
{
	pool = ThreadPool::getInstance();

	Task* task = new Task(&proxy,fd);

	pool->dispatch(task);
}

Server::Server(const char* ip, int port,const char* proxyIp, int proxyPort)
{
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

	memset(&proxy, 0, sizeof(proxy));
    proxy.sin_family = AF_INET;
    proxy.sin_addr.s_addr = inet_addr(proxyIp);
    proxy.sin_port = htons(proxyPort);
}

Server::~Server()
{
    event_base_free(base);
}

//初始化服务器
bool Server::init()
{
    //初始化event base
    base = event_base_new();

    if(!base){
        cerr << "create event_base fail !!!" << endl << flush;
        return false;
    }

    //监听ip端口
    struct evconnlistener* listener = evconnlistener_new_bind(
                base,                                           //libevent 上下文
                listenCallBack,                                 //回调函数                                    
                this,                                           //回调函数参数
                LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,        //关闭释放资源
                10,                                             //连接队列大小
                (struct sockaddr*)&server,                     //监听地址端口信息
                sizeof(server));
 
    if(!listener){
        return false;
    }

    //初始化任务处理线程池
    pool = ThreadPool::getInstance();

    if(!pool->init(16))
    {
        cerr << "init threadpool failed " << endl;
        return 0;
    }

    return true;
}

void Server::run()
{

	if(!init()){
        cerr << "init server failed " << endl;
		exit(-1);
    }
	
    event_base_dispatch(base);
}