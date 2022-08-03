//#pragma once
#ifndef CHATINFO_H
#define CHATINFO_H

#include <event.h>
#include <list>
#include "chat_database.h"

using namespace std;

#define MAXNUM    1024     //��ʾȺ������

struct User               // �����û���
{
	string name;		  // �����û���
	struct bufferevent* bev; // ÿ�������û��Ļ���������
};
typedef struct User User;

struct GroupUser	// Ⱥ��Ա��
{
	string name;
};

typedef struct GroupUser GroupUser;

struct Group	//Ⱥ����
{
	string name;	// Ⱥ������
	list<GroupUser>* l; // Ⱥ����Ⱥ��Ա����
};
typedef struct Group Group;

class Server;

class ChatInfo
{
	friend class Server; // ����Server��������ChatInfo����Ԫ�࣬���Է���ChatInfo�еĵ�������Ϣ������˽�г�Ա�ͱ�����Ա����
private:
	list<User>* online_user;     //�����������ߵ��û���Ϣ��������������ʽ
	list<Group>* group_info;     //��������Ⱥ����Ϣ������������
	ChatDataBase* mydatabase;    //���ݿ����

public:
	ChatInfo();
	~ChatInfo();

	bool info_group_exist(string);
	bool info_user_in_group(string, string); // �жϸ��û�string2�Ƿ��Ѿ��ڸ�Ⱥ��string1��
	void info_group_add_user(string, string); // ���û�string2���뵽��Ⱥ��string1�ڵ��Ⱥ��Ա������
	struct bufferevent* info_get_friend_bev(string);
	string info_get_group_member(string);	  // ��ȡȺ��string2��Ⱥ��Ա�����ظ�string1
	void info_add_new_group(string, string); // ���½�Ⱥ�ģ�Ⱥ��Ϊstring1��Ⱥ��ԱΪstring2�����뵽Ⱥ��Ϣ������
};

#endif
