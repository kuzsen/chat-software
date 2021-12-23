#include "server.h"

Server::Server(const char *ip, int port)
{
	//创建事件集合
	base = event_base_new();

	// socket网络编程知识
	struct sockaddr_in server_addr; // 使用命令man inet_addr，找到需要包含头文件#include <sys/socket.h>，#include <netinet/in.h>，#include <arpa/inet.h>
	memset(&server_addr, 0, sizeof(server_addr));  // 初始化————清空地址，memset需要包含头文件#include<string.h>
	server_addr.sin_family = AF_INET;	// 使用man socket，找到AF_INET，IPv4协议地址族，AF_INET      IPv4 Internet protocols
	server_addr.sin_port = htons(port); // 大小端转换，转换为网络字节序
	server_addr.sin_addr.s_addr = inet_addr(ip);

	//创建监听对象
	listener = evconnlistener_new_bind (base, listener_cb, NULL,
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr*)&server_addr,
		sizeof(server_addr));
	/*
		listener_cb					一个回调函数实例，，一旦有客户端发起连接，被监听对象监听到，触发回调函数，对该连接进行处理
		LEV_OPT_CLOSE_ON_FREE		连接断开时释放监听对象
		LEV_OPT_REUSEABLE			ip地址可以重复绑定（复用），调试时，当ctrl+c暂停服务器程序运行时，解决地址已经被绑定的问题
		backlog = 10				设置监听队列的长度
		(struct sockaddr *)&server_addr   强制转换
	*/
	if (NULL == listener)
	{
		cout << "evconnlistener_new_bind error！" << endl;
	}

	cout << "服务器初始化成功 开始监听客户端" << endl;
	event_base_dispatch(base);       //监听集合————循环
	//cout << "测试VS2019远程连接我的阿里云服务器，调试linux中的程序" << endl;
	
}

void Server::listener_cb (struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "接受客户端的连接，fd = " << fd << endl;

};

Server::~Server()
{
	//event_base_free(base);           //最终释放集合，否则容易造成内存泄露
}