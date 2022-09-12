//	#pragma once	Ϊ�˱���ͬһ��ͷ�ļ���������include����Σ�C/C++�������ֺ�ʵ�ַ�ʽ��һ����#ifndef��ʽ����һ����#pragma once��ʽ
#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <string.h>
#include <event.h>			//ʹ��ls /usr/include/����鿴
#include<event2/listener.h>	// evconnlistener	��	evconnlistener_new_bind ��ͷ�ļ���ʹ��ls /usr/include/event2/�鿴
#include<thread>
#include <jsoncpp/json/json.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// inet_addr������ͷ�ļ�

#include <unistd.h>// usleep()��sleep()����,�����ӳٹ�����̡����̱�����ŵ�reday queue,���ļ�����ȴ��ͻ��������ļ����������õ�
#include "chatlist.h"
using namespace std;

#define IP     "172.19.18.114" // ������
#define PORT   8000

#define MAXSIZE  1024 * 1024 

class Server
{
private:
	struct event_base* base;          //�¼�����
	struct evconnlistener* listener;    //�����¼�

	static ChatInfo* chatlist;                //������󣨺�����������
	static ChatDataBase* chatdb;              //����һ�����ݿ���󣬱���ע��ȹ���ʱ����Ҫ����user���ݿ⣬���õ�  


private: // ��̬��Ա��������̬����������(::)(�������.)����,����̬����������thisָ��,��ʶ��������,����ͨ����������ľ�̬���ݳ�Ա����
	/*��ľ�̬��Ա(�����ͷ���)�����౾��������ص�ʱ��ͻ�����ڴ棬����ͨ������ֱ��ȥ���ʣ��Ǿ�̬��Ա�������ͷ�����������Ķ�������ֻ������Ķ���������������ʵ����ʱ�Ż�����ڴ棬Ȼ��ͨ����Ķ���ʵ����ȥ���ʡ�*/
	static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
	static void client_handler(int); // �ͻ��˶�Ӧ�̴߳�����
	static void read_cb(struct bufferevent* bev, void* ctx);
	static void event_cb(struct bufferevent* bev, short what, void* ctx);

	static void send_file_handler(int, int, int*, int*); // �ļ���������Ӧ���̴߳�����


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
	Server(const char *ip = "127.0.0.1", int port = 8000); // �вι��캯��
	~Server();
};

#endif
