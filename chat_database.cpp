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
	mysql = mysql_init(NULL); // mysql��ʼ������
	mysql = mysql_real_connect(mysql, "localhost", "root", "root", name, 0, NULL, 0); // ����������mysql���ݿ⣬����ip���û��������룬name���������ݿ�����
	if (NULL == mysql)
	{
		cout << "connect database failure��" << endl;
	}

	if (mysql_query(mysql, "set names utf8;") != 0) // �������ݿ�����ʽΪutf8����ֹ��������
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
		s[count] += row[0];   // s�ַ��������б�������ݿ���ÿ��Ⱥ�ĵ�����
		count++;
	}

	return count;  // �������ݿ�Ⱥ�ĵĸ���
}

void ChatDataBase::my_database_get_group_member(string name, string& s)  //��ȡĳ��nameȺ�ĵ����г�Ա������s��
{
	char sql[1024] = { 0 };
	sprintf(sql, "select member from %s;", name.c_str()); // ����һ��mysql��ѯ���=sql������%��ֻ��Ϊchar��c_str��Ⱥ��name�ַ���ת��Ϊchar

	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql); // ��ִ�н���洢��res��
	if (NULL == res)
	{
		cout << "mysql_store_result error" << endl;
	}

	MYSQL_ROW row = mysql_fetch_row(res);// ���д洢res
	if (NULL == row[0])
	{
		return;
	}
	s += row[0]; // sȺ��Ա�ַ���������������  С��|С��|С��
}

void ChatDataBase::my_database_disconnect()
{
	mysql_close(mysql);
}

bool ChatDataBase::my_database_user_exist(string name) // �û�ע��ʱ����Ҫ�����û��Ƿ��Ѿ���user���ݿ��У�user���ݿ��Ѿ���server.cpp��ע���û�����Ӻ��ѵȺ����д�
{	/*user���ݿ��У�Ϊһ���û�����һ�ű�table�����������û��������е���Ϣ��������password������friend���ַ����������ӵ�Ⱥ��group*/

	char sql[128] = { 0 };
	sprintf(sql, "show tables like '%s';", name.c_str());//���ҵ�ǰ���ݿ����Ƿ���ڱ���=��ע����û���name

	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);

	if (NULL == row)      //���û�б�������name�ı�����û�������
	{
		return false;
	}
	else                  //�û�����
	{
		return true;
	}
}

void ChatDataBase::my_database_user_password(string name, string password)
{
	//����һ��SQL��䣬�ڵ�ǰ���ݿ�user�д���һ�ű���Ϊ�û���Ϊname�ı����д洢password��friend,chatgroup������Ϣ
	char sql[128] = { 0 };
	sprintf(sql, "create table %s (password varchar(16), friend varchar(4096), chatgroup varchar(4096)) character set utf8;", name.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "create table error" << endl;
	}

	memset(sql, 0, sizeof(sql));//���sql���
	//��spl����ǽ�password���뵽����Ϊname�ı���
	sprintf(sql, "insert into %s (password) values ('%s');",// ע�⣬��name��ո��Ѿ����������ڶ������������ý�����������,�����������Ϣ���룬��Ҫ�����ţ�����˫����֮�䲻����Ƕ��˫���ţ�����password�ӵ�����
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

bool ChatDataBase::my_database_is_friend(string n1, string n2) // �ж��û�2�Ƿ��û�1�ĺ���
{
	char sql[128] = { 0 };
	sprintf(sql, "select friend from %s;", n1.c_str()); // ʹ��n1����n1��friend�ַ������Ƿ���n2����֮Ҳ��
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (NULL == row[0]) // n1����Ϊ��NULL
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

void ChatDataBase::my_database_add_new_friend(string n1, string n2) // ���û�n2���뵽�û�n1���friend�ַ�����
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
	if (NULL == row[0])    //ԭ��û�к���
	{
		friend_list.append(n2);
	}
	else
	{
		friend_list.append(row[0]);
		friend_list += "|"; // ��ԭ������֮���ȼ���'|'
		friend_list += n2;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set friend = '%s';", n1.c_str(), friend_list.c_str()); // ����n1���friend����
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}

// ���������Ⱥ��ʱ����ǰ�жϸ�Ⱥ���Ƿ����
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
	if (NULL == row)      //Ⱥ������
	{
		return false;
	}
	else                  //Ⱥ����
	{
		return true;
	}
}

// ��`chatgroup`�д�����Ⱥ�ģ���ʼ��Ⱥ������Ⱥ����Ⱥ��Ա��Ϣ
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
// ��`user`�У���Ⱥ��string2���뵽�û�string1��Ⱥ���ַ�����
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
	if (row[0] != NULL) // ԭ��Ⱥ���ַ�����Ϊ�գ���ĩβ����"|group_name"
	{
		all_group += row[0];
		all_group += "|";
		all_group += group_name;
	}
	else //ԭ��Ⱥ���ַ���Ϊ�գ���Ⱥ��Ϊ��һ��Ⱥ�ģ�ֱ�Ӽ���
	{
		all_group += group_name;
	}

	memset(sql, 0, sizeof(sql));
	sprintf(sql, "update %s set chatgroup = '%s';", user_name.c_str(), all_group.c_str()); // ����chatgroup
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
	// �������ݿ��и�Ⱥ�ĵ�Ⱥ�ĳ�Ա�ֶ�
	sprintf(sql, "update %s set member = '%s';", group_name.c_str(), all_member.c_str());
	if (mysql_query(mysql, sql) != 0)
	{
		cout << "mysql_query error" << endl;
	}
}