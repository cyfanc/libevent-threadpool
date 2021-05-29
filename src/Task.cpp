#include "Task.h"
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

using namespace std;

extern int g_count; 
extern pthread_mutex_t g_mutex; 
extern struct timeval g_st;
extern struct timeval g_et;

Task::Task(struct sockaddr_in* proxy,int fd):proxy(proxy),sockClient(fd)
{
	
}

Task::~Task()
{
	
}

static void clientReadCallBack(struct bufferevent * bev, void* arg)
{

	Task* task = (Task*)arg;

	task->clientCallBack();
   
	return ;
}

static void serverReadCallBack(struct bufferevent * bev, void* arg)
{

	Task* task = (Task*)arg;
	
	task->serverCallBack();

	return ;
}

static void eventCallBack(struct bufferevent* bev, short which, void* arg)
{

	Task* task = (Task*)arg;

	if(which & (BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)){

		printf("\033[31m thread:%d task close\033[0m\n",task->getThreadId());

		if(task->getClientBev())
		{
			bufferevent_free(task->getClientBev());
		}
		
		if(task->getServerBev())
		{
			bufferevent_free(task->getServerBev());
		}
		
		delete task;
		
	}

	return ;
}

void Task::clientCallBack()
{
	struct evbuffer* src = bufferevent_get_input(bevClient);
	
	struct evbuffer* dst = bufferevent_get_output(bevServer);

	evbuffer_add_buffer(dst, src);

	//qps 测试
	#if 0
	{
		pthread_mutex_lock(&g_mutex);
		
		g_count++;
		
		if(g_count%100000 == 0)
		{
			gettimeofday(&g_et, NULL);
			printf("qps:%f\n",g_count*1.0/((g_et.tv_sec-g_st.tv_sec)*1000000 + g_et.tv_usec - g_st.tv_usec)*1000000);
			gettimeofday(&g_st, NULL);
			g_count = 0;
		}

		pthread_mutex_unlock(&g_mutex);
	}
	#endif
	
	return ;
}

void Task::serverCallBack()
{

	struct evbuffer* src = bufferevent_get_input(bevServer);
	
	struct evbuffer* dst = bufferevent_get_output(bevClient);

	evbuffer_add_buffer(dst, src);
	
	return ;
}


bool Task::init()
{

	return initClient() && initServer() ? true : false;

}

bool Task::initClient()
{

	bevClient = bufferevent_socket_new(base, sockClient, BEV_OPT_CLOSE_ON_FREE);

	if(!bevClient){
		
		cerr << " initClient fail!!" << endl;
		
		return false;
	}

	bufferevent_setcb(bevClient, clientReadCallBack, NULL, eventCallBack, this);

	bufferevent_enable(bevClient, EV_READ|EV_WRITE);

	timeval tm = {600,0};

	bufferevent_set_timeouts(bevClient,&tm,0);
	
	return true;
}

bool Task::initServer()
{
	
	bevServer = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
	
	if(!bevServer){		
		
		cerr << " initClient fail!!" << endl ;
				
		bufferevent_free(bevClient);
			
		return false;
	}

	if (bufferevent_socket_connect(bevServer, (struct sockaddr*)proxy, sizeof(struct sockaddr_in))<0) {
		
		cerr << "connect proxy fail" << endl ;
		
		bufferevent_free(bevServer);
		
		bufferevent_free(bevClient);
		
		return false;
    }
	
	bufferevent_setcb(bevServer, serverReadCallBack, NULL, eventCallBack, this);

	bufferevent_enable(bevServer, EV_READ|EV_WRITE);

	timeval tm = {600,0};

	bufferevent_set_timeouts(bevServer,&tm,0);
		
	return true;
}
