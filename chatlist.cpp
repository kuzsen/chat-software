#include "chatlist.h"

ChatInfo::ChatInfo()
{
	online_user = new list<User>; // ��ʼ������

	group_info = new list<Group>;

	//��group_info���������Ⱥ��Ϣ
	mydatabase = new ChatDataBase; // ����һ�����ݿ����
	mydatabase->my_database_connect("chatgroup"); // ������chatgroupȺ�����ݿ�

	string group_name[MAXNUM];
	int group_num = mydatabase->my_database_get_group_name(group_name);// ��ȡ��һ�������ϵ�chatgroup���ݿ��е�Ⱥ�ĵĸ���group_num��ÿ��Ⱥ�ĵ���������group_name

	for (int i = 0; i < group_num; i++)//��chatgroup���ݿ�������Ⱥ����Ϣ���뵽Ⱥ��Ϣ������
	{
		Group g; //����һ��Ⱥ�Ķ���
		g.name = group_name[i];
		g.l = new list<GroupUser>;    //����Ⱥ�������û�����������������������ʽ

		group_info->push_back(g);

		string member;              //����Ⱥ�������û�
		mydatabase->my_database_get_group_member(group_name[i], member); // group_name[i]ĳһȺ�����֣�����Ⱥ���е�Ⱥ��Ա�ŵ��ַ���member��
		if (member.size() == 0)    //string member = С��|С��|С��
		{
			continue;
		}

		int start = 0, end = 0;
		GroupUser u; // Ⱥ��Ա�ഴ��һ��Ⱥ��Ա����ֻ��һ��name��
		while (1)
		{
			end = member.find('|', start);
			if (-1 == end)
			{
				break;
			}
			u.name = member.substr(start, end - start);
			g.l->push_back(u);// ����ȡ����ĳһȺ�ĳ�Ա���뵽Ⱥ�ĳ�Ա������
			start = end + 1;
			u.name.clear();
		}
		u.name = member.substr(start, member.size() - start);//��member�����һ��Ⱥ�ĳ�Ա��С�š�Ҳ���뵽������
		g.l->push_back(u);

	}

	/*for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		cout << "Ⱥ���� " << it->name << endl;
		for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
		{
			cout << i->name << endl;
		}
	}*/

	mydatabase->my_database_disconnect(); // ʹ���굱ǰ���ݿ��Ͽ����ӣ�������һ������

	cout << "��ʼ������ɹ�" << endl;
}

bool ChatInfo::info_group_exist(string group_name)
{
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (it->name == group_name)
		{
			return true;
		}
	}
	return false;
}

// �ж��û��Ƿ��Ѿ��ڸ�Ⱥ����
bool ChatInfo::info_user_in_group(string group_name, string user_name)
{
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (it->name == group_name) // �ҵ���Ⱥ��
		{
			// ������Ⱥ�ĵ�Ⱥ��Ա��������û�����һһ�ȶ�
			for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++) 
			{
				if (i->name == user_name)
				{
					return true;
				}
			}
		}
	}

	return false;
}

// ���û�string2���뵽��Ⱥ�Ľڵ��Ⱥ��Ա������
void ChatInfo::info_group_add_user(string group_name, string user_name)
{
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (it->name == group_name)
		{
			GroupUser u; // �½�һ��Ⱥ��Ա�ڵ�
			u.name = user_name;
			it->l->push_back(u);
		}
	}
}

struct bufferevent* ChatInfo::info_get_friend_bev(string name)
{
	for (list<User>::iterator it = online_user->begin(); it != online_user->end(); it++)
	{
		if (it->name == name)
		{
			return it->bev;
		}
	}
	return NULL;
}

// ��ȡȺ��group��Ⱥ��Ա�����ظ�string1
string ChatInfo::info_get_group_member(string group)
{
	string member;
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (group == it->name)
		{
			for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
			{
				member += i->name;
				member += "|";
			}
		}
	}
	return member;
}

// ���½�Ⱥ�ģ�Ⱥ��Ϊstring1��Ⱥ��ԱΪstring2�����뵽Ⱥ��Ϣ������
void ChatInfo::info_add_new_group(string group_name, string user_name)
{
	Group g; // Ⱥ����ڵ�
	g.name = group_name;
	g.l = new list<GroupUser>; // Ⱥ��Ա����
	group_info->push_back(g); // �����½�Ⱥ�Ľڵ����Ⱥ��Ϣ������

	GroupUser u; 
	u.name = user_name;
	g.l->push_back(u); // ��Ⱥ���ӵ�Ⱥ��Ա������
}
