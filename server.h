//	#pragma once	为了避免同一个头文件被包含（include）多次，C/C++中有两种宏实现方式：一种是#ifndef方式；另一种是#pragma once方式
#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <string.h>
#include <event.h>			//使用ls /usr/include/命令查看
#include<event2/listener.h>	// evconnlistener	和	evconnlistener_new_bind 的头文件，使用ls /usr/include/event2/查看

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// inet_addr函数的头文件

using namespace std;

#define IP     "172.19.18.114"
#define PORT   8000

class Server
{
private:
	struct event_base* base;          //事件集合
	struct evconnlistener* listener;    //监听事件


private: // 静态成员函数
	static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
	//static void client_handler(int);


public:
	Server(const char *ip = "127.0.0.1", int port = 8000); // 有参构造函数
	~Server();
};

#endif
