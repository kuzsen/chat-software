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
	event_base_dispatch(base);       //�������ϡ�������ѭ����
	//cout << "����VS2019Զ�������ҵİ����Ʒ�����������linux�еĳ���" << endl;
	
}

void Server::listener_cb (struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "���ܿͻ��˵����ӣ�fd = " << fd << endl;

	//���������߳�������ÿͻ���
	thread client_thread(client_handler, fd);	// #include<thread>�߳���ͷ�ļ�
	client_thread.detach();    //�̷߳��룬���߳����н������Զ��ͷ���Դ
};

// ĳ�ͻ��˵Ĺ����߳�
void Server::client_handler(int fd)
{
	//����ĳһ�ͻ����¼�����
	struct event_base* base = event_base_new();

	// ����bufferevent����������
	struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev) {
		cout << "bufferevent_socket_new error!" << endl;
	}
	//��bufferevent���ûص�����
	bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);

	//ʹ�ܻص�����
	bufferevent_enable(bev, EV_READ);

	event_base_dispatch(base);    //ѭ���������ϣ������ͻ����Ƿ������ݷ��͹�������������������ͻ����˳��������½���

	event_base_free(base);
	cout << "�ͷż��ϡ��߳��˳�" << endl; // ĳһ���ӵĿͻ��˶Ͽ�ʱ����ӡ����
}
void Server::read_cb(struct bufferevent* bev, void* ctx)	// �ӿͻ��˶�ȡ���ݻص�����
{
	char buf[1024] = { 0 };
	int size = bufferevent_read(bev, buf, sizeof(buf)); // ���ӻ�����bev���������ݷŵ�buf������
	if (size < 0)
	{
		cout << "bufferevent_read error" << endl;
	}

	cout << buf << endl; // �ڷ������д�ӡ�ͻ��˷���������Ϣ�����԰������ǵ��ԣ�����߼���ϵ

	/*https://www.sojson.com/json/json_what.html             
	Json��⣬������һ��{��ֵ��}���ַ�������������ʽ/�ṹ���������ݴ���*/


	Json::Reader reader;       //����json����,����ͷ�ļ�#include <jsoncpp/json/json.h>
	Json::FastWriter writer;   //�����������ظ��ͻ��˵����ݣ����ڴ��е�Value����ת����JSON�ĵ�/����װjson����������ļ��������ַ����У����ڷ��͸��ͻ���
	Json::Value val;

	if (!reader.parse(buf, val))    //parse�ѿͻ������������JSON������ַ���ת���� json ���󣬲�������val������
	{
		cout << "��������������ʧ��" << endl;
	}

	string cmd = val["cmd"].asString();//asString()������json��ʽֱ��תΪ�ַ���������ʹ��

	if (cmd == "register")   //ע�Ṧ��
	{
		server_register(bev, val); // ��һ������Ϊ����������bev�����ĳһ���ض��ͻ��˵ģ��ڶ�������Ϊ�ͻ��˷��͵���Ϣ�����¾���
	}
	// ���¹��ܻ�û��
	else if (cmd == "login")
	{
		server_login(bev, val);
	}
	else if (cmd == "add")
	{
		server_add_friend(bev, val);
	}
	else if (cmd == "create_group")
	{
		server_create_group(bev, val);
	}
	else if (cmd == "add_group")
	{
		server_add_group(bev, val);
	}
	else if (cmd == "private_chat")
	{
		server_private_chat(bev, val);
	}
	else if (cmd == "group_chat")
	{
		server_group_chat(bev, val);
	}
	else if (cmd == "get_group_member")
	{
		server_get_group_member(bev, val);
	}
	else if (cmd == "offline")
	{
		server_user_offline(bev, val);
	}
	else if (cmd == "send_file")
	{
		server_send_file(bev, val);
	}
}

void Server::event_cb(struct bufferevent* bev, short what, void* ctx)	// �쳣����ص�����
{

}
Server::~Server()
{
	event_base_free(base);           //�����ͷż��ϣ�������������ڴ�й¶
}

void Server::server_register(struct bufferevent* bev, Json::Value val) // �������жϵ�ǰע����û��Ƿ��Ѿ���user���ݿ��У��������ڣ���Ҫ�ӽ�ȥ
{
	chatdb->my_database_connect("user"); //����������user���ݿ�,�൱�ڳ�ʼ����̬��Ա

	if (chatdb->my_database_user_exist(val["user"].asString()))   //�û�����
	{
		Json::Value val;// �½�һ��json��ʽ������,����������ֵ��
		val["cmd"] = "register_reply";// ע��ظ������ʧ��
		val["result"] = "failure";

		string s = Json::FastWriter().write(val);//���ڴ��е�Value����ת����JSON�ĵ�,������ļ��������ַ�����

		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)//int bufferevent_write(struct bufferevent *bufev,const void* data, size_t size);return 0 if successful, or -1 if an error occurred
		{
			cout << "bufferevent_write error!" << endl;
		}
	}
	else                                               //�û�������
	{
		chatdb->my_database_user_password(val["user"].asString(), val["password"].asString());//��ע���û��������룬���뵽user���ݿ���
		
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "success";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error!" << endl;
		}
	}

	chatdb->my_database_disconnect();//ע������ر����ݿ⣨user��
}

//���»�δ��
void Server::server_login(struct bufferevent* bev, Json::Value val)
{
	chatdb->my_database_connect("user");
	if (!chatdb->my_database_user_exist(val["user"].asString()))   //�û�������
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "user_not_exist";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error" << endl;
		}
		return; // ��¼ʧ�ܣ�������������¼����
	}

	if (!chatdb->my_database_password_correct(val["user"].asString(),
		val["password"].asString()))    //���벻��ȷ
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "password_error";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error" << endl;
		}
		return; // ��¼ʧ�ܣ�������������¼����
	}

	Json::Value v;
	string s, name;

	//�������û���������Ӹ��û�
	User u = { val["user"].asString(), bev };
	chatlist->online_user->push_back(u);

	//��ȡ���Ѻ�Ⱥ���б��ҷ���
	string friend_list, group_list;
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	v["cmd"] = "login_reply";
	// v["currentLoginUser"] = val["user"].asString(); // lzs������¼�ɹ��󣬣����ڿͻ���ʵ���û���¼�󣬽��û�������Ϊ�û��������
	// Ҳ���Դ�qt�ͻ��ˣ�widge���ڵ�¼������û������ٴ��ݸ��û���������棬�������Ӻ��ѣ�����Ⱥ�ģ�Ⱥ����Ⱥ��Ա���ǵ�ǰ�û����ȶ��õĵ�
	v["result"] = "success";
	v["friend"] = friend_list;
	v["group"] = group_list;
	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	//����ѷ�����������
	int start = 0, end = 0, flag = 1;
	while (flag)
	{
		end = friend_list.find('|', start);
		if (-1 == end)
		{
			name = friend_list.substr(start, friend_list.size() - start);
			flag = 0;//���������һ�����ѣ��˳�ѭ��
		}
		else
		{
			name = friend_list.substr(start, end - start);
		}

		for (list<User>::iterator it = chatlist->online_user->begin();
			it != chatlist->online_user->end(); it++)
		{
			if (name == it->name) // �жϺ����Ƿ����ߣ�������ߣ������it->name���ͣ��Լ�����������
			{
				v.clear();
				v["cmd"] = "friend_login";
				v["friend"] = val["user"]; // ��¼�û�user�Ǳ��˵ĺ���
				s = Json::FastWriter().write(v);
				if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)   // �����õ�¼�û����������ߺ���
				{
					cout << "bufferevent_write" << endl;
				}
			}
		}
		start = end + 1;
	}

	chatdb->my_database_disconnect();
}

void  Server::server_add_friend(struct bufferevent* bev, Json::Value val)
// �ͻ��˷��ͣ�{"cmd":"add", "user":"С��", "friend":"С��"};
{
	Json::Value v;
	string s;

	chatdb->my_database_connect("user");

	if (!chatdb->my_database_user_exist(val["friend"].asString()))   //��ӵĺ���friend������
	{
		v["cmd"] = "add_reply";
		v["result"] = "user_not_exist"; // �ظ��ͻ���

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // ���ˣ���Ӻ��Ѳ�������
	}
	

	if (chatdb->my_database_is_friend(val["user"].asString(), val["friend"].asString())) // �Ƿ��Ѿ��Ǻ���
	{
		v.clear();
		v["cmd"] = "add_reply";
		v["result"] = "already_friend"; // �ظ��ͻ���

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return;
	}

	//1�޸�˫�������ݿ�,�˴�û��ʵ�ֺ�����֤�Ĺ��ܣ�������Ҫ�Ż�����
	chatdb->my_database_add_new_friend(val["user"].asString(), val["friend"].asString());
	chatdb->my_database_add_new_friend(val["friend"].asString(), val["user"].asString());


	// 2�ظ�ִ����Ӻ��ѵ��û�����Ӻ��ѳɹ�
	v.clear();
	v["cmd"] = "add_reply";
	v["result"] = "success";
	v["friend"] = val["friend"];

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	// 3���������û����ж���ӵĺ����Ƿ���ڣ������ڣ��ظ�����bev�����Ѿ���ִ����Ӻ���������û�С�������Ϊ����
	for (list<User>::iterator it = chatlist->online_user->begin();
		it != chatlist->online_user->end(); it++)
	{
		if (val["friend"] == it->name)
		{
			v.clear();
			v["cmd"] = "add_friend_reply";
			v["result"] = val["user"];

			s = Json::FastWriter().write(v);
			if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)
			{
				cout << "bufferevent_write" << endl;
			}
		}
	}

	chatdb->my_database_disconnect();
}

void Server::server_create_group(struct bufferevent* bev, Json::Value val)
{
	chatdb->my_database_connect("chatgroup");

	//�ж�Ⱥ�Ƿ��Ѿ�����
	if (chatdb->my_database_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "create_group_reply";
		v["result"] = "group_exist"; // �Ѿ�����

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return;
	}

	//��Ⱥ��Ϣд�����ݿ�
	chatdb->my_database_add_new_group(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("user");
	//��`user`�У��޸Ĵ�����Ⱥ���û���Ⱥ���ַ���
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
	// ���½�Ⱥ�ģ�����Ⱥ����Ⱥ��Ա��Ⱥ�������뵽Ⱥ��Ϣ������
	chatlist->info_add_new_group(val["group"].asString(), val["user"].asString());

	// �ظ��ͻ����û���Ⱥ�Ĵ����ɹ�
	Json::Value value;
	value["cmd"] = "create_group_reply";
	value["result"] = "success";
	value["group"] = val["group"];

	string s = Json::FastWriter().write(value);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// �û����Ⱥ��
void Server::server_add_group(struct bufferevent* bev, Json::Value val)
{
	//�ж�Ⱥ�Ƿ����
	if (!chatlist->info_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "group_not_exist"; // �ظ��ͻ����û�����Ҫ��ӵ�Ⱥ�Ĳ�����

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // ����
	}

	//�ж��û��Ƿ��Ѿ��ڸ�Ⱥ�����ʱʹ��Ⱥ����Ϣ���������ٶ���������ݿ����
	if (chatlist->info_user_in_group(val["group"].asString(), val["user"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "user_in_group"; // �ظ��ͻ����û������Ѿ������ڸ�Ⱥ����

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // ����
	}

	//�޸����ݿ⣨�û��� Ⱥ��
	chatdb->my_database_connect("user");
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());-
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("chatgroup");
	chatdb->my_database_group_add_user(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	//�޸�Ⱥ����Ϣ���������û����뵽��Ⱥ�Ľڵ��Ⱥ��Ա������
	chatlist->info_group_add_user(val["group"].asString(), val["user"].asString());

	Json::Value v;
	v["cmd"] = "add_group_reply";
	v["result"] = "success"; // �ظ��ͻ����û������Ⱥ�ĳɹ�
	v["group"] = val["group"];

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// ˽��
void Server::server_private_chat(struct bufferevent* bev, Json::Value val)
{
	// �����жϺ����Ƿ����ߡ����������������û���������仺��������
	struct bufferevent* to_bev = chatlist->info_get_friend_bev(val["user_to"].asString());
	if (NULL == to_bev) // ˵�����Ѳ�����
	{
		Json::Value v;
		v["cmd"] = "private_chat_reply";
		v["result"] = "offline";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // ����
	}

	// ���û�����������bev�е�����valת��Ϊ�ַ���s����ת�������ѵĻ���������to_bev
	string s = Json::FastWriter().write(val);
	if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	Json::Value v;
	v["cmd"] = "private_chat_reply"; // �ظ����ͷ���˽�ĳɹ�
	v["result"] = "success";

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// Ⱥ��
void Server::server_group_chat(struct bufferevent* bev, Json::Value val)
{
	// ����Ⱥ����Ϣ�����ҵ���Ⱥ��
	for (list<Group>::iterator it = chatlist->group_info->begin(); it != chatlist->group_info->end(); it++)
	{
		if (val["group"].asString() == it->name)
		{
			// ������Ⱥ�ĵ�Ⱥ��Ա���������Լ���
			for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
			{
				// ��ȡ����Ⱥ��Ա�Ļ���������ת�����ͷ���bev
				struct bufferevent* to_bev = chatlist->info_get_friend_bev(i->name);
				if (to_bev != NULL)
				{
					string s = Json::FastWriter().write(val);
					if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
					{
						cout << "bufferevent_write" << endl;
					}
				}
			}
			break; // �ҵ���Ⱥ�ģ���ֹͣ����
		}
	}

	Json::Value v;
	v["cmd"] = "group_chat_reply";
	v["result"] = "success"; // �ظ����ͷ��û���Ⱥ�ķ��ͳɹ�

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// ��ȡĳ��Ⱥ�ĵ����г�Ա�������ظ���Ⱥ��
void Server::server_get_group_member(struct bufferevent* bev, Json::Value val)
{
	string member = chatlist->info_get_group_member(val["group"].asString());

	Json::Value v;
	v["cmd"] = "get_group_member_reply";
	v["member"] = member;
	v["group"] = val["group"];// ���ظ���Ⱥ��

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

}

// �û����ߡ�����������ĺô�֮һ�����㴦��������λ��ɾ������
void Server::server_user_offline(struct bufferevent* bev, Json::Value val)
{
	//���û�����������ɾ�����û�
	for (list<User>::iterator it = chatlist->online_user->begin();
		it != chatlist->online_user->end(); it++)
	{
		if (it->name == val["user"].asString())
		{
			chatlist->online_user->erase(it); // �����û�����ɾ�����û��Ľڵ�
			break;
		}
	}

	chatdb->my_database_connect("user");

	//��ȡ���ѣ�Ⱥ���б��ҷ��ظ�friend_list, group_list�����������÷�ʽ����
	string friend_list, group_list;
	string name, s;
	Json::Value v;
	
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	// �Ȼ�������к���(ͨ��'|'��ȡ�����б��ַ����õ�ĳ������)�������ߺ�������Աȣ�ֻ�����ߵĺ��ѷ��͸��û�����������
	int start = 0, end = 0, flag = 1;
	while (flag)
	{
		end = friend_list.find('|', start);
		if (-1 == end)
		{
			name = friend_list.substr(start, friend_list.size() - start);
			flag = 0;
		}
		else
		{
			name = friend_list.substr(start, end - start);
		}

		for (list<User>::iterator it = chatlist->online_user->begin();
			it != chatlist->online_user->end(); it++)
		{
			if (name == it->name)
			{
				v.clear();
				v["cmd"] = "friend_offline";
				v["friend"] = val["user"];
				s = Json::FastWriter().write(v);
				if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)
				{
					cout << "bufferevent_write" << endl;
				}
			}
		}
		start = end + 1;
	}

	chatdb->my_database_disconnect();
}

// �����ļ�
void Server::server_send_file(struct bufferevent* bev, Json::Value val)
{
	Json::Value v;
	string s;

	//�ж϶Է����ļ����շ����Ƿ�����
	struct bufferevent* to_bev = chatlist->info_get_friend_bev(val["to_user"].asString());
	if (NULL == to_bev)
	{
		v["cmd"] = "send_file_reply";
		v["result"] = "offline"; // �ظ����ͷ������շ�������
		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // ����
	}

	//�������̣߳������ļ���������������ļ��������񣨲�Ӱ������ȹ��ܣ�
	int port = 8080; // portΪ�˿ںţ�ʵ����Ӧ���������һ��ϵͳ��δʹ�õ�port������Կͻ���֮�䴫�����ݵ�ʱ��ᴴ������ļ�����������Ȼ���ܹ���һ���˿ںţ��ᷢ����ͻ���˴�������Ϊ�˼�
	int from_fd = 0, to_fd = 0; // ���÷�ʽ��÷��ͷ��ͻ��˺ͽ��շ��ͻ������ļ�������֮���fd����ͬ�ڿͻ��������������֮���fd�����潫������ļ���������fd�Ƿ��޸ģ��ж��Ƿ��пͻ������䷢������
	thread send_file_thread(send_file_handler, val["length"].asInt(), port, &from_fd, &to_fd); // lengthΪ�ļ���С,asIntת��Ϊ����
	send_file_thread.detach(); // ������ɺ��̷߳��룬�ͷ���Դ

	// ���ļ��������˿ںŷ��ظ����Ϳͻ��ˣ����Ϳͻ��˸���port���ļ��������������ӣ�
	v.clear();
	v["cmd"] = "send_file_port_reply";
	v["port"] = port; // 
	v["filename"] = val["filename"]; // ����qt�ͻ���ʵ���ļ�����
	v["length"] = val["length"];
	s = Json::FastWriter().write(v);

	//*********************�˴���Ƶ�л�δ����
	////if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // bufferevent_write�з�������4KB���ƣ������������json���ֶη��ͣ�������������ȡ�����׳��ְַ���ճ������
	if (send(bev->ev_read.ev_fd, s.c_str(), strlen(s.c_str()), 0) < 0) // send���������ļ������������ļ����ݣ������������������ƣ����ļ������ַ���ֱ�ӷ��ͣ������Ƿ�װjson��ʽ��
	{
		cout << "bufferevent_write" << endl;
	}

	int count = 0; // �ȴ�ʱ��
	while (from_fd <= 0) // ���жϷ��Ϳͻ��������ļ��������Ƿ�ɹ�
	{
		// ���ĳһʱ�̣����Ϳͻ������������ļ�����������send_file_handler�е�accept()ִ�гɹ�����from_fd���޸ģ��˳�while
		count++; 
		usleep(100000); // ��Ҫ����ͷ�ļ�����usleep() ��sleep()����,�����ӳٹ�����̡����̱�����ŵ�reday queue,����ÿ�ι���100000΢�� = 100����
		if (count == 100) // 100 * 100 = 10000���� = 10�룬���ȴ�10��֮��from_fd��Ȼ<=0��˵����δ���ӳɹ����ж�Ϊ���ӳ�ʱ�����ط��Ϳͻ��������ļ���������ʱ
		{
			pthread_cancel(send_file_thread.native_handle());   //ȡ���ļ����������ڵ��̣߳�native_handle()����������������̺߳�
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout"; // �����ļ���������ʱ
			s = Json::FastWriter().write(v);
			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // bev
			{
				cout << "bufferevent_write" << endl;
			}
			return; // ����
		}
	}
	/*���ˣ����Ϳͻ������ӷ������ɹ������������жϽ��տͻ����Ƿ����ļ����������ӳɹ� */

	//���ļ��������˿ںţ����ظ����տͻ���
	v.clear();
	v["cmd"] = "recv_file_port_reply";
	v["port"] = port;
	v["filename"] = val["filename"];
	v["length"] = val["length"];
	s = Json::FastWriter().write(v);

	//if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
	if (send(to_bev->ev_read.ev_fd, s.c_str(), strlen(s.c_str()), 0) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	count = 0;
	while (to_fd <= 0) // �жϽ��տͻ��������ļ��������Ƿ�ɹ���ͬ��
	{
		// ���ĳһʱ�̣����տͻ������������ļ�����������send_file_handler�е�accept()ִ�гɹ�����to_fd���޸ģ��˳�while
		count++;
		usleep(100000);
		if (count == 100)
		{
			pthread_cancel(send_file_thread.native_handle());   //ȡ���߳�
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout";
			s = Json::FastWriter().write(v);

			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // Ҳ�ǻظ����Ϳͻ��ˣ���Ϊ���ļ��������ֱ��������ͻ������ӣ������ĸ����ӳ�ʱ�����Ƿ����ļ�ʧ�ܣ�����տͻ���û�й�ϵ��ֻ�ûظ����Ϳͻ���ʧ�ܵ�ԭ���ǣ������ļ���������ʱ����
			{
				cout << "bufferevent_write" << endl;
			}
		}
	}
	/*���ˣ����տͻ���Ҳ���ļ����������ӳɹ����������ļ�������һ�ߴ�*f_fd���գ�һ����*t_fd���ͣ���ִ���ļ����乤�� */
}

// �ļ����������
void Server::send_file_handler(int length, int port, int* f_fd, int* t_fd)
{
	/*
	Socketԭ���⡪�������ǳ��ŵ�����
	https://blog.csdn.net/pashanhu6402/article/details/96428887?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164214549216780269869952%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164214549216780269869952&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-96428887.pc_search_insert_ulrmf&utm_term=socket&spm=1018.2226.3001.4187
	*/


	// ����һ��socket������/�֣�socket descriptor������Ψһ��ʶһ��socket�����socket�����ָ��ļ�������һ���������Ĳ��������õ�����������Ϊ������ͨ����������һЩ��д����
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/*
		AF_INET��Э���壬ͨ���б�����ö�Ӧ�ĵ�ַ����AF_INET������Ҫ��ipv4��ַ��32λ�ģ���˿ںţ�16λ�ģ������
		SOCK_STREAM socket���ͣ�tcp������
		protocolĬ��Ϊ0��Э�飬��protocolΪ0ʱ�����Զ�ѡ��type���Ͷ�Ӧ��Ĭ��Э��
	
	*/
	if (-1 == sockfd)
	{
		return;
	}

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// ���ջ�����
	int nRecvBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	//���ͻ�����
	int nSendBuf = MAXSIZE;    //����Ϊ1M
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); // bind()������һ����ַ���е��ض���ַ����socket
	listen(sockfd, 10);

	/*
	accept()����
	TCP�ͻ������ε���socket()��connect()֮�����TCP������������һ����������TCP�������������������֮�󣬾ͻ����accept()����ȡ���������������Ӿͽ������ˡ�֮��Ϳ��Կ�ʼ����I/O�����ˣ�����ͬ����ͨ�ļ��Ķ�дI/O������

	*/
	int len = sizeof(client_addr);
	//���ܷ��Ϳͻ��˵���������������ӳɹ���*f_fd�ᱻ�޸ģ�����ʱΪ0��������server_send_file�������жϷ��Ϳͻ����Ƿ�����ļ����������ӳɹ�
	*f_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&len); // (struct sockaddr*)ǿ������ת����&ȡ��ַ��(socklen_t*)ǿ������ת����C���ã�C++�ã�
	//���ܽ��տͻ��˵���������
	*t_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&len);


	// �ļ�������һ�ߴ�*f_fd���գ�һ����*t_fd����
	char buf[MAXSIZE] = { 0 }; // MAXSIZE = 4096��ÿ�δ������ջ��ͣ�4096���ֽ�=4K 
	size_t size, sum = 0;
	while (1) 
	{
		/*
		https://blog.csdn.net/yu_yuan_1314/article/details/9766137?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164215100316780357285801%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164215100316780357285801&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-2-9766137.pc_search_insert_ulrmf&utm_term=recv%E5%92%8Csend&spm=1018.2226.3001.4187
		Socket��recv()��send()�������
	    ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		ssize_t send(int sockfd, const void *buf, size_t len, int flags);
		*/

		size = recv(*f_fd, buf, MAXSIZE, 0); // ÿ�ν��մ�С������buf�ַ�������
		if (size <= 0 || size > MAXSIZE) // ������ν��������쳣��˵����ȡ��ϣ��˳���дwhileѭ��
		{
			break;
		}
		sum += size; // �ۼ�ÿ�ζ�ȡ��С
		send(*t_fd, buf, size, 0); // ��ÿ�ν��յ���ʵ�ʴ�СΪsize���洢��buf�ַ������е����ݷ��ͳ�ȥ
		if (sum >= length) // ��ǰ�����ļ���Сsum >= �ļ������Ĵ�С�����ͽ������˳�ѭ�� 
		{
			break;
		}
		memset(buf, 0, MAXSIZE); // ѭ����ʹ�����飬ÿ�ζ�ȡ���ͽ��������buf�ַ����飬���ڴ洢�´ζ�ȡ��������
	}
	// ���ˣ������ļ����乤����ɣ��رշ�������տͻ������ļ�������֮��������֣�
	close(*f_fd); 
	close(*t_fd);
	close(sockfd); // �رո��ļ���������ע�⣺close����ֻ��ʹ��Ӧsocket�����ֵ����ü���-1��ֻ�е����ü���Ϊ0��ʱ�򣬲Żᴥ��TCP�ͻ����������������ֹ��������

	/*���ˣ�send_file_thread����ִ�н�������������server_send_file�����У�ִ����һ���̷߳��뺯��send_file_thread.detach()*/
}