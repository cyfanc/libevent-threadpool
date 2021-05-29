#include "Server.h"
#include <iostream>

using namespace std;
int g_count = 0; 
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER; 
struct timeval g_st;
struct timeval g_et;

int main(int argc, char** argv)
{
	gettimeofday(&g_st, NULL);

	Server* server = new Server("0.0.0.0",8888,"172.198.1.25",3306);

    	server->run();

   	return 0;
}
