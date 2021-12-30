# chat_server
基于阿里云服务器+libevent+qt+mysql+Json等实现仿qq聊天软件，主要功能包括注册、登录、添加好友、聊天（私聊和群聊）、文件传输、建群、加群、好友上下线提醒。
## 1、购买阿里云服务器，配置环境，详见文件***
## 2、初始化用户和群聊信息存储方式
在mysql中，创建user和chatgroup两个数据库，
其中user存储用户信息，为一个用户创建一个表名为用户名的表，表中包含用户的密码password、friend好友、groupchat所在群聊
chatgroup存储群聊信息，为每个群聊创建一张表名为群聊名的表，表中包含群聊的群主owner、群聊成员member
创建online_user和group_info两个链表————————链表访问速度比数据库快，且方便后续功能的实现
其中，online_user存储在线用户的信息，节点为<user>，包括在线用户名和在线用户的缓冲区对象bev
group_info存储群聊信息，节点为<Group>，包括群聊名称和群成员链表，其中群成员链表的节点为<GroupUser>，只包含群成员姓名