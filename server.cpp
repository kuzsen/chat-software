#include "server.h"

Server::Server(const char *ip, int port)
{
	//创建事件集合
	base = event_base_new();

	// socket网络编程知识
	struct sockaddr_in server_addr; // 使用命令man inet_addr，找到需要包含头文件#include <sys/socket.h>，#include <netinet/in.h>，#include <arpa/inet.h>
	memset(&server_addr, 0, sizeof(server_addr));  // 初始化————清空地址，memset需要包含头文件#include<string.h>
	server_addr.sin_family = AF_INET;	// 使用man socket，找到AF_INET，IPv4协议地址族，AF_INET      IPv4 Internet protocols
	server_addr.sin_port = htons(port); // 大小端转换，转换为网络字节序
	server_addr.sin_addr.s_addr = inet_addr(ip);

	//创建监听对象
	listener = evconnlistener_new_bind (base, listener_cb, NULL,
		LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, (struct sockaddr*)&server_addr,
		sizeof(server_addr));
	/*
		listener_cb					一个回调函数实例，，一旦有客户端发起连接，被监听对象监听到，触发回调函数，对该连接进行处理
		LEV_OPT_CLOSE_ON_FREE		连接断开时释放监听对象
		LEV_OPT_REUSEABLE			ip地址可以重复绑定（复用），调试时，当ctrl+c暂停服务器程序运行时，解决地址已经被绑定的问题
		backlog = 10				设置监听队列的长度
		(struct sockaddr *)&server_addr   强制转换
	*/
	if (NULL == listener)
	{
		cout << "evconnlistener_new_bind error！" << endl;
	}

	cout << "服务器初始化成功 开始监听客户端" << endl;
	event_base_dispatch(base);       //监听集合————循环。
	//cout << "测试VS2019远程连接我的阿里云服务器，调试linux中的程序" << endl;
	
}

void Server::listener_cb (struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
	cout << "接受客户端的连接，fd = " << fd << endl;

	//创建工作线程来处理该客户端
	thread client_thread(client_handler, fd);	// #include<thread>线程类头文件
	client_thread.detach();    //线程分离，当线程运行结束后，自动释放资源
};

// 某客户端的工作线程
void Server::client_handler(int fd)
{
	//创建某一客户端事件集合
	struct event_base* base = event_base_new();

	// 创建bufferevent缓存区对象
	struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bev) {
		cout << "bufferevent_socket_new error!" << endl;
	}
	//给bufferevent设置回调函数
	bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);

	//使能回调函数
	bufferevent_enable(bev, EV_READ);

	event_base_dispatch(base);    //循环监听集合（监听客户端是否有数据发送过来），程序卡在这里，当客户端退出，则往下进行

	event_base_free(base);
	cout << "释放集合、线程退出" << endl; // 某一连接的客户端断开时，打印此行
}
void Server::read_cb(struct bufferevent* bev, void* ctx)	// 从客户端读取数据回调函数
{
	char buf[1024] = { 0 };
	int size = bufferevent_read(bev, buf, sizeof(buf)); // 将从缓存区bev读到的数据放到buf数组中
	if (size < 0)
	{
		cout << "bufferevent_read error" << endl;
	}

	cout << buf << endl; // 在服务器中打印客户端发过来的消息，可以帮助我们调试，理解逻辑关系

	/*https://www.sojson.com/json/json_what.html             
	Json详解，本身是一种{键值对}的字符串，的数据形式/结构，用于数据传输*/


	Json::Reader reader;       //解析json对象,包含头文件#include <jsoncpp/json/json.h>
	Json::FastWriter writer;   //将服务器返回给客户端的数据（将内存中的Value对象，转换成JSON文档/）封装json对象，输出到文件或者是字符串中，便于发送给客户端
	Json::Value val;

	if (!reader.parse(buf, val))    //parse把客户端输入的满足JSON规则的字符串转换成 json 对象，并保存在val（）中
	{
		cout << "服务器解析数据失败" << endl;
	}

	string cmd = val["cmd"].asString();//asString()函数将json格式直接转为字符串，便于使用

	if (cmd == "register")   //注册功能
	{
		server_register(bev, val); // 第一个参数为缓存区对象bev，针对某一个特定客户端的，第二个参数为客户端发送的信息，以下均是
	}
	// 以下功能还没看
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

void Server::event_cb(struct bufferevent* bev, short what, void* ctx)	// 异常处理回调函数
{

}
Server::~Server()
{
	event_base_free(base);           //最终释放集合，否则容易造成内存泄露
}

void Server::server_register(struct bufferevent* bev, Json::Value val) // 接下来判断当前注册的用户是否已经在user数据库中，若不存在，需要加进去
{
	chatdb->my_database_connect("user"); //首先连接上user数据库,相当于初始化静态成员

	if (chatdb->my_database_user_exist(val["user"].asString()))   //用户存在
	{
		Json::Value val;// 新建一个json格式的数据,往里面加入键值对
		val["cmd"] = "register_reply";// 注册回复，结果失败
		val["result"] = "failure";

		string s = Json::FastWriter().write(val);//将内存中的Value对象转换成JSON文档,输出到文件或者是字符串中

		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)//int bufferevent_write(struct bufferevent *bufev,const void* data, size_t size);return 0 if successful, or -1 if an error occurred
		{
			cout << "bufferevent_write error!" << endl;
		}
	}
	else                                               //用户不存在
	{
		chatdb->my_database_user_password(val["user"].asString(), val["password"].asString());//将注册用户名和密码，加入到user数据库中
		
		Json::Value val;
		val["cmd"] = "register_reply";
		val["result"] = "success";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write error!" << endl;
		}
	}

	chatdb->my_database_disconnect();//注册结束关闭数据库（user）
}

//以下还未看
void Server::server_login(struct bufferevent* bev, Json::Value val)
{
	chatdb->my_database_connect("user");
	if (!chatdb->my_database_user_exist(val["user"].asString()))   //用户不存在
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "user_not_exist";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 登录失败，立即结束掉登录函数
	}

	if (!chatdb->my_database_password_correct(val["user"].asString(),
		val["password"].asString()))    //密码不正确
	{
		Json::Value val;
		val["cmd"] = "login_reply";
		val["result"] = "password_error";

		string s = Json::FastWriter().write(val);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 登录失败，立即结束掉登录函数
	}

	Json::Value v;
	string s, name;

	//向链表中添加用户
	User u = { val["user"].asString(), bev };
	chatlist->online_user->push_back(u);

	//获取好友和群聊列表并且返回
	string friend_list, group_list;
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	v["cmd"] = "login_reply";
	// v["currentLoginUser"] = val["user"].asString(); // lzs——登录成功后，，便于客户端实现用户登录后，将用户名设置为用户界面标题
	// 也可以从qt客户端，widge窗口登录输入的用户名，再传递给用户的聊天界面，后面的添加好友，创建群聊（群主和群成员都是当前用户）等都用的到
	v["result"] = "success";
	v["friend"] = friend_list;
	v["group"] = group_list;
	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	//向好友发送上线提醒
	int start = 0, end = 0, flag = 1;
	while (flag)
	{
		end = friend_list.find('|', start);
		if (-1 == end)
		{
			name = friend_list.substr(start, friend_list.size() - start);
			flag = 0;//遍历完最后一个好友，退出循环
		}
		else
		{
			name = friend_list.substr(start, end - start);
		}

		for (list<User>::iterator it = chatlist->online_user->begin();
			it != chatlist->online_user->end(); it++)
		{
			if (name == it->name) // 判断好友是否在线，如果在线，向好友it->name发送，自己的上线提醒
			{
				v.clear();
				v["cmd"] = "friend_login";
				v["friend"] = val["user"]; // 登录用户user是别人的好友
				s = Json::FastWriter().write(v);
				if (bufferevent_write(it->bev, s.c_str(), strlen(s.c_str())) < 0)   // 发给该登录用户的所有在线好友
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
// 客户端发送：{"cmd":"add", "user":"小明", "friend":"小华"};
{
	Json::Value v;
	string s;

	chatdb->my_database_connect("user");

	if (!chatdb->my_database_user_exist(val["friend"].asString()))   //添加的好友friend不存在
	{
		v["cmd"] = "add_reply";
		v["result"] = "user_not_exist"; // 回复客户端

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 至此，添加好友操作结束
	}
	

	if (chatdb->my_database_is_friend(val["user"].asString(), val["friend"].asString())) // 是否已经是好友
	{
		v.clear();
		v["cmd"] = "add_reply";
		v["result"] = "already_friend"; // 回复客户端

		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return;
	}

	//1修改双方的数据库,此处没有实现好友验证的功能，后期需要优化补充
	chatdb->my_database_add_new_friend(val["user"].asString(), val["friend"].asString());
	chatdb->my_database_add_new_friend(val["friend"].asString(), val["user"].asString());


	// 2回复执行添加好友的用户，添加好友成功
	v.clear();
	v["cmd"] = "add_reply";
	v["result"] = "success";
	v["friend"] = val["friend"];

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	// 3遍历在线用户，判断添加的好友是否存在，若存在，回复它的bev，你已经被执行添加好友命令的用户小明，添加为好友
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

	//判断群是否已经存在
	if (chatdb->my_database_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "create_group_reply";
		v["result"] = "group_exist"; // 已经存在

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return;
	}

	//把群信息写入数据库
	chatdb->my_database_add_new_group(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("user");
	//在`user`中，修改创建该群聊用户的群聊字符串
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
	// 将新建群聊（包括群名和群成员即群主）加入到群信息链表中
	chatlist->info_add_new_group(val["group"].asString(), val["user"].asString());

	// 回复客户端用户，群聊创建成功
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

// 用户添加群聊
void Server::server_add_group(struct bufferevent* bev, Json::Value val)
{
	//判断群是否存在
	if (!chatlist->info_group_exist(val["group"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "group_not_exist"; // 回复客户端用户，你要添加的群聊不存在

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 结束
	}

	//判断用户是否已经在该群聊里，此时使用群聊信息链表，访问速度相比于数据库更快
	if (chatlist->info_user_in_group(val["group"].asString(), val["user"].asString()))
	{
		Json::Value v;
		v["cmd"] = "add_group_reply";
		v["result"] = "user_in_group"; // 回复客户端用户，你已经存在于该群聊中

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 结束
	}

	//修改数据库（用户表 群表）
	chatdb->my_database_connect("user");
	chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());-
	chatdb->my_database_disconnect();

	chatdb->my_database_connect("chatgroup");
	chatdb->my_database_group_add_user(val["group"].asString(), val["user"].asString());
	chatdb->my_database_disconnect();

	//修改群聊信息链表，将该用户加入到该群聊节点的群成员链表中
	chatlist->info_group_add_user(val["group"].asString(), val["user"].asString());

	Json::Value v;
	v["cmd"] = "add_group_reply";
	v["result"] = "success"; // 回复客户端用户，添加群聊成功
	v["group"] = val["group"];

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// 私聊
void Server::server_private_chat(struct bufferevent* bev, Json::Value val)
{
	// 首先判断好友是否在线————遍历在线用户链表，获得其缓存区对象，
	struct bufferevent* to_bev = chatlist->info_get_friend_bev(val["user_to"].asString());
	if (NULL == to_bev) // 说明好友不在线
	{
		Json::Value v;
		v["cmd"] = "private_chat_reply";
		v["result"] = "offline";

		string s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 结束
	}

	// 将用户缓存区对象bev中的内容val转换为字符串s，再转发给好友的缓存区对象to_bev
	string s = Json::FastWriter().write(val);
	if (bufferevent_write(to_bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

	Json::Value v;
	v["cmd"] = "private_chat_reply"; // 回复发送方，私聊成功
	v["result"] = "success";

	s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// 群聊
void Server::server_group_chat(struct bufferevent* bev, Json::Value val)
{
	// 遍历群聊信息链表找到该群聊
	for (list<Group>::iterator it = chatlist->group_info->begin(); it != chatlist->group_info->end(); it++)
	{
		if (val["group"].asString() == it->name)
		{
			// 遍历该群聊的群成员链表（包括自己）
			for (list<GroupUser>::iterator i = it->l->begin(); i != it->l->end(); i++)
			{
				// 获取在线群成员的缓存区对象，转发发送方的bev
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
		}
	}

	Json::Value v;
	v["cmd"] = "group_chat_reply";
	v["result"] = "success"; // 回复发送方用户，群聊发送成功

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}
}

// 获取某个群聊成员，并返回给该群聊
void Server::server_get_group_member(struct bufferevent* bev, Json::Value val)
{
	string member = chatlist->info_get_group_member(val["group"].asString());

	Json::Value v;
	v["cmd"] = "get_group_member_reply";
	v["member"] = member;
	v["group"] = val["group"];// 返回给该群聊

	string s = Json::FastWriter().write(v);
	if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
	{
		cout << "bufferevent_write" << endl;
	}

}

// 用户下线
void Server::server_user_offline(struct bufferevent* bev, Json::Value val)
{
	//从用户在线链表中删除该用户
	for (list<User>::iterator it = chatlist->online_user->begin();
		it != chatlist->online_user->end(); it++)
	{
		if (it->name == val["user"].asString())
		{
			chatlist->online_user->erase(it); // 在线用户链表，删除该用户的节点
			break;
		}
	}

	chatdb->my_database_connect("user");

	//获取好友，群聊列表并且返回给friend_list, group_list，参数是引用方式传递
	string friend_list, group_list;
	string name, s;
	Json::Value v;
	
	chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list);

	//向在线好友，发送该用户的下线提醒
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

// 发送文件
void Server::server_send_file(struct bufferevent* bev, Json::Value val)
{
	Json::Value v;
	string s;

	//判断对方（文件接收方）是否在线
	struct bufferevent* to_bev = chatlist->info_get_friend_bev(val["to_user"].asString());
	if (NULL == to_bev)
	{
		v["cmd"] = "send_file_reply";
		v["result"] = "offline"; // 回复发送方，接收方不在线
		s = Json::FastWriter().write(v);
		if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0)
		{
			cout << "bufferevent_write" << endl;
		}
		return; // 结束
	}

	//启动新线程，创建文件服务器，处理该文件传输任务（不影响聊天等功能）
	int port = 8080; // port为端口号，实际上应该随机生成一个系统中未使用的port，当多对客户端之间传输数据的时候会创建多个文件服务器，显然不能共用一个端口号，会发生冲突，此处仅仅是为了简化
	int from_fd = 0, to_fd = 0; // 引用方式获得发送方客户端和接收方客户端与文件服务器之间的fd，不同于客户端与聊天服务器之间的fd，后面将会根据文件服务器的fd是否被修改，判断是否有客户端向其发起连接
	thread send_file_thread(send_file_handler, val["length"].asInt(), port, &from_fd, &to_fd); // length为文件大小,asInt转换为整形
	send_file_thread.detach(); // 传输完成后，线程分离，释放资源

	// 将文件服务器端口号返回给发送客户端，发送客户端根据port向文件服务器发起连接，
	v.clear();
	v["cmd"] = "send_file_port_reply";
	v["port"] = port; // 
	v["filename"] = val["filename"];
	v["length"] = val["length"];
	s = Json::FastWriter().write(v);

	//*********************此处视频中还未讲到
	////if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // bufferevent_write有发送容量4KB限制，且如果不满足json的字段发送，服务器连续读取，容易出现分包、粘包问题
	if (send(bev->ev_read.ev_fd, s.c_str(), strlen(s.c_str()), 0) < 0) // send函数，向文件服务器发送文件数据，发送数据无容量限制，将文件当作字符串直接发送（而不是封装json格式）
	{
		cout << "bufferevent_write" << endl;
	}

	int count = 0; // 等待时间
	while (from_fd <= 0) // 先判断发送客户端连接文件服务器是否成功
	{
		// 如果某一时刻，发送客户端连接上了文件服务器，即send_file_handler中的accept()执行成功，则from_fd被修改，退出while
		count++; 
		usleep(100000); // 需要包含头文件——usleep() 与sleep()类似,用于延迟挂起进程。进程被挂起放到reday queue,设置每次挂起100000微秒 = 100毫秒
		if (count == 100) // 100 * 100 = 10000毫秒 = 10秒，即等待10秒之后，from_fd仍然<=0，说明仍未连接成功，判定为连接超时，返回发送客户端连接文件服务器超时
		{
			pthread_cancel(send_file_thread.native_handle());   //取消文件服务器所在的线程，native_handle()函数————获得线程号
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout"; // 连接文件服务器超时
			s = Json::FastWriter().write(v);
			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // bev
			{
				cout << "bufferevent_write" << endl;
			}
			return; // 结束
		}
	}
	/*至此，发送客户端连接服务器成功，接下来再判断接收客户端是否与文件服务器连接成功 */

	//将文件服务器端口号，返回给接收客户端
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
	while (to_fd <= 0) // 判断接收客户端连接文件服务器是否成功，同上
	{
		// 如果某一时刻，接收客户端连接上了文件服务器，即send_file_handler中的accept()执行成功，则to_fd被修改，退出while
		count++;
		usleep(100000);
		if (count == 100)
		{
			pthread_cancel(send_file_thread.native_handle());   //取消线程
			v.clear();
			v["cmd"] = "send_file_reply";
			v["result"] = "timeout";
			s = Json::FastWriter().write(v);

			if (bufferevent_write(bev, s.c_str(), strlen(s.c_str())) < 0) // 也是回复发送客户端，因为是文件服务器分别与两个客户端连接，不管哪个连接超时，都是发送文件失败，与接收客户端没有关系，只用回复发送客户端失败的原因是：连接文件服务器超时即可
			{
				cout << "bufferevent_write" << endl;
			}
		}
	}
	/*至此，接收客户端也与文件服务器连接成功，接下来文件服务器一边从*f_fd接收，一边向*t_fd发送，即执行文件传输工作 */
}

// 文件传输服务器
void Server::send_file_handler(int length, int port, int* f_fd, int* t_fd)
{
	/*
	Socket原理讲解————非常号的文章
	https://blog.csdn.net/pashanhu6402/article/details/96428887?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164214549216780269869952%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164214549216780269869952&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-96428887.pc_search_insert_ulrmf&utm_term=socket&spm=1018.2226.3001.4187
	*/


	// 创建一个socket描述符/字（socket descriptor），它唯一标识一个socket。这个socket描述字跟文件描述字一样，后续的操作都有用到它，把它作为参数，通过它来进行一些读写操作
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/*
		AF_INET，协议族，通信中必须采用对应的地址，如AF_INET决定了要用ipv4地址（32位的）与端口号（16位的）的组合
		SOCK_STREAM socket类型，tcp基于流
		protocol默认为0，协议，当protocol为0时，会自动选择type类型对应的默认协议
	
	*/
	if (-1 == sockfd)
	{
		return;
	}

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 接收缓冲区
	int nRecvBuf = MAXSIZE;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	//发送缓冲区
	int nSendBuf = MAXSIZE;    //设置为1M
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(IP);
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); // bind()函数把一个地址族中的特定地址赋给socket
	listen(sockfd, 10);

	/*
	accept()函数
	TCP客户端依次调用socket()、connect()之后就想TCP服务器发送了一个连接请求。TCP服务器监听到这个请求之后，就会调用accept()函数取接收请求，这样连接就建立好了。之后就可以开始网络I/O操作了，即类同于普通文件的读写I/O操作。

	*/
	int len = sizeof(client_addr);
	//接受发送客户端的连接请求，如果连接成功，*f_fd会被修改（传入时为0），可在server_send_file函数中判断发送客户端是否与该文件服务器连接成功
	*f_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&len); // (struct sockaddr*)强制类型转换，&取地址，(socklen_t*)强制类型转换（C不用，C++用）
	//接受接收客户端的连接请求
	*t_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&len);


	// 文件服务器一边从*f_fd接收，一边向*t_fd发送
	char buf[MAXSIZE] = { 0 }; // MAXSIZE = 4096，每次处理（接收或发送）4096个字节=4K 
	size_t size, sum = 0;
	while (1) 
	{
		/*
		https://blog.csdn.net/yu_yuan_1314/article/details/9766137?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522164215100316780357285801%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=164215100316780357285801&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~sobaiduend~default-2-9766137.pc_search_insert_ulrmf&utm_term=recv%E5%92%8Csend&spm=1018.2226.3001.4187
		Socket中recv()与send()函数详解
	    ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		ssize_t send(int sockfd, const void *buf, size_t len, int flags);
		*/

		size = recv(*f_fd, buf, MAXSIZE, 0); // 每次接收大小，放在buf字符数组中
		if (size <= 0 || size > MAXSIZE) // 如果单次接收数据异常，说明读取完毕，退出读写while循环
		{
			break;
		}
		sum += size; // 累加每次读取大小
		send(*t_fd, buf, size, 0); // 将每次接收到的实际大小为size，存储在buf字符数组中的数据发送出去
		if (sum >= length) // 当前传输文件大小sum >= 文件本来的大小，发送结束，退出循环 
		{
			break;
		}
		memset(buf, 0, MAXSIZE); // 循环中使用数组，每次读取发送结束后，清空buf字符数组，便于存储下次读取到的数据
	}
	// 至此，本次文件传输工作完成，关闭发送与接收客户端与文件服务器之间的描述字，
	close(*f_fd); 
	close(*t_fd);
	close(sockfd); // 关闭该文件服务器（注意：close操作只是使相应socket描述字的引用计数-1，只有当引用计数为0的时候，才会触发TCP客户端向服务器发送终止连接请求）

	/*至此，send_file_thread函数执行结束，接下来在server_send_file函数中，执行下一个线程分离函数send_file_thread.detach()*/
}