//#pragma once
#ifndef CHATINFO_H
#define CHATINFO_H

#include <event.h>
#include <list>
#include "chat_database.h"

using namespace std;

#define MAXNUM    1024     //表示群最大个数

struct User               // 在线用户类
{
	string name;		  // 在线用户名
	struct bufferevent* bev; // 每个在线用户的缓冲区对象
};
typedef struct User User;

struct GroupUser	// 群成员类
{
	string name;
};

typedef struct GroupUser GroupUser;

struct Group	//群聊类
{
	string name;	// 群聊名称
	list<GroupUser>* l; // 群聊中群成员链表
};
typedef struct Group Group;

class Server;

class ChatInfo
{
	friend class Server;
private:
	list<User>* online_user;     //保存所有在线的用户信息————链表形式
	list<Group>* group_info;     //保存所有群聊信息————链表
	ChatDataBase* mydatabase;    //数据库对象

public:
	ChatInfo();
	~ChatInfo();

	bool info_group_exist(string);
	bool info_user_in_group(string, string); // 判断该用户string2是否已经在该群聊string1中
	void info_group_add_user(string, string); // 将用户string2加入到该群聊string1节点的群成员链表中
	struct bufferevent* info_get_friend_bev(string);
	string info_get_group_member(string);	  // 获取群聊string2的群成员，返回给string1
	void info_add_new_group(string, string); // 将新建群聊（群名为string1，群成员为string2）加入到群信息链表中
};

#endif
