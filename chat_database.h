//#pragma once
#ifndef CHAT_DATABASE_H
#define CHAT_DATABASE_H

#include <mysql/mysql.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

class ChatDataBase
{
private:
	MYSQL* mysql; // ����һ������mysql���ݿ���󣬰���ͷ�ļ�#include <mysql/mysql.h>
public:
	ChatDataBase();
	~ChatDataBase();

	void my_database_connect(const char* name);                    // ����ĳ�����ݿ⣨���ݿ����֣�
	int my_database_get_group_name(string*);                       // ��ȡȺ�ĸ�������������,�ŵ�����string��
	void my_database_get_group_member(string, string&);            // ��ȡ��һ��������Ⱥ�����ƣ���Ⱥ��Ա�ŵ��ڶ���������
	bool my_database_user_exist(string);						   // ���ĳ�û��Ƿ������user���ݿ���
	void my_database_user_password(string, string);
	bool my_database_password_correct(string, string);
	bool my_database_is_friend(string, string);					   // �ж��û�2�Ƿ��û�2�ĺ��ѣ���Ӻ��ѹ���ʱ��Ҫ��ǰ�ж�һ��
	void my_database_get_friend_group(string, string&, string&);
	void my_database_add_new_friend(string, string);			   // ���û�2���뵽�û�1���friend�����ַ�����
	bool my_database_group_exist(string);
	void my_database_add_new_group(string, string);
	void my_database_user_add_group(string, string);
	void my_database_group_add_user(string, string);
	void my_database_disconnect();								   // �Ͽ���ǰ���ӵ����ݿ�
};

#endif

