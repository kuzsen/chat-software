#include "server.h"

Server::Server(const char *ip, int port)
{
	//�����¼�����
	base = event_base_new();

	// socket������֪ʶ
	struct sockaddr_in server_addr; // ʹ������man inet_addr���ҵ���Ҫ����ͷ�ļ�#include <sys/socket.h>��#include <netinet/in.h>��#include <arpa/inet.h>
	memset(&server_addr, 0, sizeof(server_addr));  // ��ʼ������������յ�ַ��memset��Ҫ����ͷ�ļ�#include<string.h>
	server_addr.sin_family = AF_INET;	// ʹ��man socket���ҵ�AF_INET��IPv4Э���ַ�壬AF_INET      IPv4 Internet protocols
	server_addr.sin_port = htons(port); // ��С��ת����ת��Ϊ�����ֽ���
	server_addr.sin_addr.s_addr = inet_addr(ip);

	//������������
	listener = evconnlistener_new_bind (base, listener_cb, NULL,
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr*)&server_addr,
		sizeof(server_addr));
	/*
		listener_cb					һ���ص�����ʵ������һ���пͻ��˷������ӣ�����������������������ص��������Ը����ӽ��д���
		LEV_OPT_CLOSE_ON_FREE		���ӶϿ�ʱ�ͷż�������
		LEV_OPT_REUSEABLE			ip��ַ�����ظ��󶨣����ã�������ʱ����ctrl+c��ͣ��������������ʱ�������ַ�Ѿ����󶨵�����
		backlog = 10				���ü������еĳ���
		(struct sockaddr *)&server_addr   ǿ��ת��
	*/
	if (NULL == listener)
	{
		cout << "evconnlistener_new_bind error��" << endl;
	}

	cout << "��������ʼ���ɹ� ��ʼ�����ͻ���" << endl;
	event_base_dispatch(base);       //�������ϡ�������ѭ��
	//cout << "����VS2019Զ�������ҵİ����Ʒ�����������linux�еĳ���" << endl;
	
}

void Server::listener_cb (struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "���ܿͻ��˵����ӣ�fd = " << fd << endl;

};

Server::~Server()
{
	//event_base_free(base);           //�����ͷż��ϣ�������������ڴ�й¶
}