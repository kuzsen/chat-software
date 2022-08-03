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
	void my_database_user_password(string, string);                // �û���½ʱ��֤����
	bool my_database_password_correct(string, string);			   // �жϵ�¼�û�string1����������string2�Ƿ���ȷ����user���ݿ��и��û����е�password�ȶ�
	bool my_database_is_friend(string, string);					   // �ж��û�2�Ƿ��û�2�ĺ��ѣ���Ӻ��ѹ���ʱ��Ҫ��ǰ�ж�һ��
	void my_database_get_friend_group(string, string&, string&);   // //��ȡĳ�û�string1�ĺ��Ѻ�Ⱥ���б��ֱ�洢��string2��string3�У����÷�ʽ����
	void my_database_add_new_friend(string, string);			   // ���û�2���뵽�û�1���friend�����ַ�����
	bool my_database_group_exist(string);						   // ���������Ⱥ��ʱ����ǰ�жϸ�Ⱥ���Ƿ����
	void my_database_add_new_group(string, string);				   // ��`chatgroup`�д�����Ⱥ�ģ���ʼ��Ⱥ������Ⱥ����Ⱥ��Ա��Ϣ
	void my_database_user_add_group(string, string);			   // ��`user`�У���Ⱥ��string2���뵽�û�string1��Ⱥ���ַ�����
	void my_database_group_add_user(string, string);			   // ��`chatgroup`���ݿ��У����û�string2���뵽Ⱥ��string1��Ⱥ��Ա�ַ�����
	void my_database_disconnect();								   // �Ͽ���ǰ���ӵ����ݿ�
};

#endif

