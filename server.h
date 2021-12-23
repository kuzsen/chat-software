//	#pragma once	Ϊ�˱���ͬһ��ͷ�ļ���������include����Σ�C/C++�������ֺ�ʵ�ַ�ʽ��һ����#ifndef��ʽ����һ����#pragma once��ʽ
#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <string.h>
#include <event.h>			//ʹ��ls /usr/include/����鿴
#include<event2/listener.h>	// evconnlistener	��	evconnlistener_new_bind ��ͷ�ļ���ʹ��ls /usr/include/event2/�鿴

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// inet_addr������ͷ�ļ�

using namespace std;

#define IP     "172.19.18.114"
#define PORT   8000

class Server
{
private:
	struct event_base* base;          //�¼�����
	struct evconnlistener* listener;    //�����¼�


private: // ��̬��Ա����
	static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
	//static void client_handler(int);


public:
	Server(const char *ip = "127.0.0.1", int port = 8000); // �вι��캯��
	~Server();
};

#endif
