#include "chat_database.h"

ChatDataBase::ChatDataBase()
{
}

ChatDataBase::~ChatDataBase()
{
	mysql_close(mysql);
}

void ChatDataBase::my_database_connect(const char* name)
{
	mysql = mysql_init(NULL); // mysql初始化函数
	mysql = mysql_real_connect(mysql, "localhost", "root", "root", name, 0, NULL, 0); // 连接真正的mysql数据库，主机ip，用户名，密码，name待连接数据库名字
	if (NULL == mysql)
	{
		cout << "connect database failure！" << endl;
	}

	if (mysql_query(mysql, "set names utf8;") != 0) // 设置数据库编码格式为utf8，防止中文乱码
	{
		cout << "mysql_query error" << endl;
	}
}

int ChatDataBase::my_database_get_group_name(string* s)
{
	if (mysql_query(mysql, "show tables;") != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	if (NULL == res)
	{
		cout << "mysql_store_result" << endl;
	}

	int count = 0;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(res))
	{
		s[count] += row[0];   // s字符串数组中保存该数据库中每个群聊的名称
		count++;
	}

	return count;  // 返回数据库群聊的个数
}

void ChatDataBase::my_database_get_group_member(string name, string& s)  //获取某个name群聊的所有成员，放在s中
{
	char sql[1024] = { 0 };
	sprintf(sql, "select member from %s;", name.c_str()); // 定义一条mysql查询语句=sql，其中%后只能为char，c_str将群聊name字符串转换为char

	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql); // 将执行结果存储到res中
	if (NULL == res)
	{
		cout << "mysql_store_result error" << endl;
	}

	MYSQL_ROW row = mysql_fetch_row(res);// 按行存储res
	if (NULL == row[0])
	{
		return;
	}
	s += row[0]; // s群成员字符串——————  小明|小李|小张
}

void ChatDataBase::my_database_disconnect()
{
	mysql_close(mysql);
}

bool ChatDataBase::my_database_user_exist(string name) // 用户注册时，需要检查该用户是否已经在user数据库中，user数据库已经在server.cpp中注册用户，添加好友等函数中打开
{	/*user数据库中，为一个用户创建一张表table，表名就是用户名，表中的信息包括密码password，好友friend（字符串），所加的群聊group*/

	char sql[128] = { 0 };
	sprintf(sql, "show tables like '%s';", name.c_str());//查找当前数据库中是否存在表名=待注册的用户名name

	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);

	if (NULL == row)      //如果没有表名等于name的表，则该用户不存在
	{
		return false;
	}
	else                  //用户存在
	{
		return true;
	}
}

void ChatDataBase::my_database_user_password(string name, string password)
{
	//定义一条SQL语句，在当前数据库user中创建一张表名为用户名为name的表，表中存储password，friend,chatgroup三条信息
	char sql[128] = { 0 };
	sprintf(sql, "create table %s (password varchar(16), friend varchar(4096), chatgroup varchar(4096)) character set utf8;", name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "create table error" << endl;
	}

	memset(sql, 0, sizeof(sql));//清空sql语句
	//该spl语句是将password加入到表名为name的表中
	sprintf(sql, "insert into %s (password) values ('%s');",// 注意，该name表刚刚已经创建，现在对它操作，不用将表名加引号,而待插入的信息密码，需要加引号，由于双引号之间不能再嵌套双引号，所以password加单引号
		name.c_str(), password.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "insert into 'password' error" << endl;
	}
}

bool ChatDataBase::my_database_password_correct(string name, string password)
{
	char sql[128] = { 0 };
	sprintf(sql, "select password from %s;", name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (password == row[0])
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ChatDataBase::my_database_get_friend_group(string name, string& f, string& g)
{
	char sql[128] = { 0 };
	sprintf(sql, "select friend from %s;", name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row[0] != NULL)
	{
		f.append(row[0]);
	}
	mysql_free_result(res);

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "select chatgroup from %s;", name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
	res = mysql_store_result(mysql);
	row = mysql_fetch_row(res);
	if (row[0] != NULL)
	{
		g.append(row[0]);
	}
}

bool ChatDataBase::my_database_is_friend(string n1, string n2) // 判断用户2是否用户1的好友
{
	char sql[128] = { 0 };
	sprintf(sql, "select friend from %s;", n1.c_str()); // 使用n1表，查n1的friend字符串中是否有n2，反之也可
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (NULL == row[0]) // n1好友为空NULL
	{
		return false;
	}
	else
	{
		string all_friend(row[0]);
		int start = 0, end = 0;
		while (1)
		{
			end = all_friend.find('|', start);
			if (-1 == end)
			{
				break;
			}
			if (n2 == all_friend.substr(start, end - start))
			{
				return true;
			}

			start = end + 1;
		}

		if (n2 == all_friend.substr(start, all_friend.size() - start))
		{
			return true;
		}
	}

	return false;
}

void ChatDataBase::my_database_add_new_friend(string n1, string n2) // 将用户n2加入到用户n1表的friend字符串中
{
	char sql[1024] = { 0 };
	sprintf(sql, "select friend from %s;", n1.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query" << endl;
	}
	string friend_list;
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (NULL == row[0])    //原来没有好友
	{
		friend_list.append(n2);
	}
	else
	{
		friend_list.append(row[0]);
		friend_list += "|"; // 与原来好友之间先加上'|'
		friend_list += n2;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set friend = '%s';", n1.c_str(), friend_list.c_str()); // 更新n1表的friend好友
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}

bool ChatDataBase::my_database_group_exist(string group_name)
{
	char sql[128] = { 0 };
	sprintf(sql, "show tables like '%s';", group_name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (NULL == row)      //群不存在
	{
		return false;
	}
	else                  //群存在
	{
		return true;
	}
}

void ChatDataBase::my_database_add_new_group(string group_name, string owner)
{
	char sql[128] = { 0 };
	sprintf(sql, "create table %s (owner varchar(32), member varchar(4096)) character set utf8;", group_name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "insert into %s values ('%s', '%s');", group_name.c_str(), owner.c_str(), owner.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}

void ChatDataBase::my_database_user_add_group(string user_name, string group_name)
{
	char sql[1024] = { 0 };
	sprintf(sql, "select chatgroup from %s;", user_name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	string all_group;
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row[0] != NULL)
	{
		all_group += row[0];
		all_group += "|";
		all_group += group_name;
	}
	else
	{
		all_group += group_name;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set chatgroup = '%s';", user_name.c_str(), all_group.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}

void ChatDataBase::my_database_group_add_user(string group_name, string user_name)
{
	char sql[1024] = { 0 };
	sprintf(sql, "select member from %s;", group_name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	string all_member;
	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row[0] != NULL)
	{
		all_member += row[0];
		all_member += "|";
		all_member += user_name;
	}
	else
	{
		all_member += user_name;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set member = '%s';", group_name.c_str(), all_member.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}