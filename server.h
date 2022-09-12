//	#pragma once	为了避免同一个头文件被包含（include）多次，C/C++中有两种宏实现方式：一种是#ifndef方式；另一种是#pragma once方式
#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <string.h>
#include <event.h>			//使用ls /usr/include/命令查看
#include<event2/listener.h>	// evconnlistener	和	evconnlistener_new_bind 的头文件，使用ls /usr/include/event2/查看
#include<thread>
#include <jsoncpp/json/json.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// inet_addr函数的头文件

#include <unistd.h>// usleep()与sleep()类似,用于延迟挂起进程。进程被挂起放到reday queue,在文件传输等待客户端连接文件服务器会用到
#include "chatlist.h"
using namespace std;

#define IP     "172.19.18.114" // 内网？
#define PORT   8000

#define MAXSIZE  1024 * 1024 

class Server
{
private:
	struct event_base* base;          //事件集合
	struct evconnlistener* listener;    //监听事件

	static ChatInfo* chatlist;                //链表对象（含有两个链表）
	static ChatDataBase* chatdb;              //创建一个数据库对象，比如注册等功能时，需要访问user数据库，会用到  


private: // 静态成员函数，静态函数由类名(::)(或对象名.)调用,但静态函数不传递this指针,不识别对象个体,所以通常用来对类的静态数据成员操作
	/*类的静态成员(变量和方法)属于类本身，在类加载的时候就会分配内存，可以通过类名直接去访问；非静态成员（变量和方法）属于类的对象，所以只有在类的对象产生（创建类的实例）时才会分配内存，然后通过类的对象（实例）去访问。*/
	static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
	static void client_handler(int); // 客户端对应线程处理函数
	static void read_cb(struct bufferevent* bev, void* ctx);
	static void event_cb(struct bufferevent* bev, short what, void* ctx);

	static void send_file_handler(int, int, int*, int*); // 文件服务器对应的线程处理函数


	static void server_register(struct bufferevent* bev, Json::Value val);
	static void server_login(struct bufferevent* bev, Json::Value val);
	static void server_add_friend(struct bufferevent* bev, Json::Value val);
	static void server_create_group(struct bufferevent* bev, Json::Value val);
	static void server_add_group(struct bufferevent* bev, Json::Value val);
	static void server_private_chat(struct bufferevent* bev, Json::Value val);
	static void server_group_chat(struct bufferevent* bev, Json::Value val);
	static void server_get_group_member(struct bufferevent* bev, Json::Value val);
	static void server_user_offline(struct bufferevent* bev, Json::Value val);
	static void server_send_file(struct bufferevent* bev, Json::Value val);

public:
	Server(const char *ip = "127.0.0.1", int port = 8000); // 有参构造函数
	~Server();
};

#endif
