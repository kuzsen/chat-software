#include "chatlist.h"

ChatInfo::ChatInfo()
{
	online_user = new list<User>;

	group_info = new list<Group>;

	//往group_info链表中添加群信息
	mydatabase = new ChatDataBase; // 创建一个数据库对象
	mydatabase->my_database_connect("chatgroup"); // 连接上chatgroup群聊数据库

	string group_name[MAXNUM];
	int group_num = mydatabase->my_database_get_group_name(group_name);// 获取上一步连接上的chatgroup数据库中的群聊的个数group_num和每个群聊的名字数组group_name

	for (int i = 0; i < group_num; i++)//将chatgroup数据库中所有群聊信息插入到群信息链表中
	{
		Group g; //创建一个群聊对象
		g.name = group_name[i];
		g.l = new list<GroupUser>;    //保存群中所有用户————————链表形式

		group_info->push_back(g);

		string member;              //保存群里所有用户
		mydatabase->my_database_get_group_member(group_name[i], member); // group_name[i]某一群聊名字，将该群聊中的群成员放到字符串member中
		if (member.size() == 0)    //string member = 小明|小李|小张
		{
			continue;
		}

		int start = 0, end = 0;
		GroupUser u; // 群成员类创建一个群成员对象（只有一个name）
		while (1)
		{
			end = member.find('|', start);
			if (-1 == end)
			{
				break;
			}
			u.name = member.substr(start, end - start);
			g.l->push_back(u);// 将截取到的某一群聊成员插入到群聊成员链表中
			start = end + 1;
			u.name.clear();
		}
		u.name = member.substr(start, member.size() - start);//将member中最后一个群聊成员“小张”也插入到链表中
		g.l->push_back(u);

	}

	/*for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		cout << "群名字 " << it->name << endl;
		for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
		{
			cout << i->name << endl;
		}
	}*/

	mydatabase->my_database_disconnect(); // 使用完当前数据库后断开连接，方便下一次连接

	cout << "初始化链表成功" << endl;
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

// 判断用户是否已经在该群聊里
bool ChatInfo::info_user_in_group(string group_name, string user_name)
{
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (it->name == group_name) // 找到该群聊
		{
			// 遍历该群聊的群成员链表，与该用户姓名一一比对
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

// 将用户string2加入到该群聊节点的群成员链表中
void ChatInfo::info_group_add_user(string group_name, string user_name)
{
	for (list<Group>::iterator it = group_info->begin(); it != group_info->end(); it++)
	{
		if (it->name == group_name)
		{
			GroupUser u; // 新建一个群成员节点
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

// 将新建群聊（群名为string1，群成员为string2）加入到群信息链表中
void ChatInfo::info_add_new_group(string group_name, string user_name)
{
	Group g; // 群聊类节点
	g.name = group_name;
	g.l = new list<GroupUser>; // 群成员链表
	group_info->push_back(g); // 将该新建群聊节点加入群信息链表中

	GroupUser u; 
	u.name = user_name;
	g.l->push_back(u); // 将群主加到群成员链表中
}
