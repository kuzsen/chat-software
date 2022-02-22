## chat-software
基于阿里云服务器+libevent+qt+mysql+Json等实现仿qq聊天软件，主要功能包括注册、登录、添加好友、聊天（私聊和群聊）、文件传输、建群、加群、好友上下线提醒。

主要分为两大部分：

第一，在linux上实现服务器端chat_server开发

第二，在qt上实现客户端chat_client开发

## 1. chat_server

### 1.1 配置服务器端开发环境

购买阿里云服务器，配置环境，详见文件——在阿里云服务器上搭建开发环境.txt

### 1.2 创建服务器与客户端接口文档

首先创建一个满足Json数据格式的服务器与客户端传输信息或者交互接口文档，方便查看重要信息且有助于接下来实现各种功能。详见文件——服务器与客户端接口文档.txt

### 1.3 封装数据库类和链表类，用于存储用户和群聊信息

在mysql中，创建user和chatgroup两个数据库，
其中user存储用户信息，为一个用户创建一个表名为用户名的表，表中包含用户的密码password、friend好友、groupchat所在群聊
chatgroup存储群聊信息，为每个群聊创建一张表名为群聊名的表，表中包含群聊的群主owner、群聊成员member

![](D:\MyCode_C++\C++资料\chat_server板书+接口文档\数据库.png)

创建online_user和group_info两个链表————————链表访问速度比数据库快，且方便后续功能的实现
其中，online_user存储在线用户的信息，节点为<user>，包括在线用户名和在线用户的缓冲区对象bev
group_info存储群聊信息，节点为<Group>，包括群聊名称和群成员链表，其中群成员链表的节点为<GroupUser>，只包含群成员姓名

![](D:\MyCode_C++\C++资料\chat_server板书+接口文档\链表.png)

### 1.4 封装一个服务器类class Server

首先初始化服务器，使其可以监听待连接的客户端：在Server的构造函数中，基于libevent创建事件集合，创建并绑定监听对象，设置监听队列的长度，即最多同时与多少客户端建立TCP连接，开始循环监听，一旦有客户端发起连接，则调用回调函数，在回调函数中创建工作线程来处理该客户端。在某一工作线程中，创建该连接客户端的事件集合，基于该事件集合创建bufferevent缓存区对象，给bufferevent设置回调函数，循环监听集合（监听客户端是否有数据发送过来），一旦从客户端读取到数据，调用回调函数，处理客户端发送的满足Json格式的数据，接下来根据解析到的不同”cmd“，实现服务器端不同的功能，比如注册功能对应server_register(bev, val)函数，当客户端退出或连接断开，释放该连接客户端的事件集合，该工作线程随之退出分离，释放资源。在Server的成员变量中，分别创建一个链表对象static ChatInfo* chatlist和一个数据库对象static ChatDataBase* chatdb，用于访问用户和群聊的相关信息。

### 1.5 实现注册功能

server_register(bev, val); // 参数1为服务器与该客户端之间的缓存区对象，参数2为客户端发送的信息

- 首先连接上user数据库，chatdb->my_database_connect("user");

- 判断注册用户是否已经存在，chatdb->my_database_user_exist(val["user"].asString()))

  - 若已经存在，回复客户端注册失败；

  - 若不存在，执行以下函数，将注册用户的用户名和密码，加入到user数据库中；然后回复客户端注册成功。

    ```c++
    chatdb->my_database_user_password(val["user"].asString(), val["password"].asString());
    ```

- 关闭user数据库，chatdb->my_database_disconnect();

### 1.6 实现用户登录功能

- 首先连接上user数据库，最后再断开连接，同上，接下来涉及到访问user数据库，均需要这两步，不再赘述；

- 判断登录用户是否已经存在于user数据库中：

  - 若不存在，回复客户端该登录用户不存在；

  - 若存在，判断用户登录密码是否正确:

    ```c++
    chatdb->my_database_password_correct(val["user"].asString(),
    val["password"].asString()
    ```

    - 若不正确，回复客户端，输入登录失败，密码不正确，将登录函数结束掉return

    - 若正确：

      - 向在线用户链表中加入该登录用户，chatlist->online_user->push_back(u);

      - 获取该登录用户好友friend_list和群聊列表group_list并且返回给客户端，便于qt客户端开发时，显示登录用户的好友和群聊列表，以实现接下来的好友上线提醒、聊天等功能；

        ```
        chatdb->my_database_get_friend_group(val["user"].asString(), friend_list, group_list)
        ```

      - 登录成功后，向该登录用户的所有在线好友，发送自己上线的提醒：遍历friend_list，与 chatlist->online_user的一一匹配，若某好友在线，则向其bev/客户端发送自己上线的提醒

### 1.7 实现添加好友功能

- 首先判断用户user——小明将要添加的好友friend小华——是否存在于用户数据库中

  - 若不存在，回复用户小明，你要添加的好友小华不存在，添加好友操作结束return

  - 若存在，再判断两人是否已经是好友关系

    ```c++
    chatdb->my_database_is_friend(val["user"].asString(), val["friend"].asString())
    ```

    - 若已经是好友关系，回复用户小明，你与添加的好友小华已经是好友关系，添加好友操作结束return

    - 若还不是好友关系：

      - 修改双方的用户表的friend好友字符串，互相添加为好友（注意，此处未考虑好友验证通过的功能，后期优化再实现）；

      ```c++
      chatdb->my_database_add_new_friend(val["user"].asString(), val["friend"].asString());
      chatdb->my_database_add_new_friend(val["friend"].asString(), val["user"].asString());
      ```

      - 回复执行添加好友的用户小明，添加好友成功
      - 遍历在线用户链表，判断添加的好友小华是否在线，若在线，回复小华，小明已将你添加为好友。

### 1.8 实现创建群聊功能

* 首先连接上`chatgroup`数据库，使用结束再断开连接，接下来涉及到访问`chatgroup`数据库，均需要这两步，不再赘述；

* 判断群是否已存在`chatdb->my_database_group_exist(val["group"].asString())`

  * 若已经存在，回复客户端用户，该群已存在

  * 若不存在，

    * 在`chatgroup`中创建该群聊，初始化群聊名，群主，群成员信息

      ```c++
      chatdb->my_database_add_new_group(val["group"].asString(), val["user"].asString())
      ```

    * 在`user`中，将新建群聊加入到创建该群聊用户的群聊字符串中

      ```c++
      chatdb->my_database_user_add_group(val["user"].asString(), val["group"].asString());
      ```

    * 将新建群聊加入到群信息链表中

      ```C++
      chatlist->info_add_new_group(val["group"].asString(), val["user"].asString());
      ```

    * 回复客户端用户，群聊创建成功

### 1.9 添加群聊 `server_add_group`

* 首先判断该群聊是否存在于`chatgroup`数据库中

  * 如果不存在，回复客户端用户，你要添加的群聊不存在，`return`；

  * 如果存在，判断该用户是否已经在该群聊中，此时使用群聊信息链表，访问速度相比于数据库更快

    ```C++
    chatlist->info_user_in_group(val["group"].asString(), val["user"].asString())
    ```

    * 如果该用户已经存在于该群聊中，回复客户端用户，你已经存在于该群聊中，`return`；

    * 若不在该群聊中

      * 在`user`中，将该群聊加入到该用户的群聊字符串中；

        在`chatgroup`中，将该用户加入到该群聊的群成员字符串中；

      * 修改群聊信息链表，将该用户加入到该群聊节点的群成员链表中；

        ```c++
        chatlist->info_group_add_user(val["group"].asString(), val["user"].asString());
        ```

      *  回复客户端用户，添加群聊成功

### 1.10 私聊 `server_private_chat`

* 首先判断好友是否在线————遍历在线用户链表，获得其缓存区对象`to_bev`

  ```c++
  struct bufferevent* to_bev = chatlist->info_get_friend_bev(val["user_to"].asString());
  ```

  * 如果`NULL == to_bev`,说明好友不在线，回复发送方，return
  * 如果好友在线，将发送方用户缓存区对象bev中的内容val转换为字符串s，再转发给好友的缓存区对象to_bev，再回复发送方，私聊发送成功。

### 1.11 群聊 `server_group_chat`

* 首先遍历群聊信息链表找到该群聊
* 遍历该群聊的群成员链表
* 遍历在线用户链表，找到每个群成员（包括发送方自己）的缓存区对象`to_bev`,若不为空，转发发送方的bev，最后回复发送方，群聊发送成功

### 1.12 获取群聊成员 `server_get_group_member`

* 遍历群聊信息链表，获取某群聊的群聊成员字符串member，将其返回给调用该函数的客户端

  ```c++
  string member = chatlist->info_get_group_member(val["group"].asString());
  ```

	### 1.13 用户下线 `server_user_offline`

* 首先从用户在线链表`online_user`中删除该用户
* 获取该用户好友friend_list
* 向在线好友，发送该用户的下线提醒

### 1.14 发送文件 server_send_file

* 首先判断对方是否在线

  * 若不在线，回复发送方，接收方不在线，return；

  * 若在线：

    * 启动新线程，创建文件服务器，处理该文件传输任务（不影响聊天等功能）,引用方式获得发送方客户端和接收方客户端相对于文件服务器的fd
  
      ```c++
      thread send_file_thread(send_file_handler, val["length"].asInt(), port, &from_fd, &to_fd);
      ```
  
    * 将文件服务器端口号返回给发送客户端，发送客户端根据端口号向文件服务器发起连接;
  
    * 判断发送客户端连接文件服务器是否成功，文件服务器中的`accept()`一旦被执行，对应的`fd`就会被修改（传入时为0），设置最长连接时间为`10s`，即等待10秒之后，`from_fd<=0`，说明仍未连接成功，判定为连接超时，取消文件服务器所在的线程，返回发送客户端连接文件服务器超时；相同方法连接接收客户端与文件服务器。
  
    * 文件服务器一边从`*f_fd`接收，一边向`*t_fd`发送，文件发送结束后，关闭发送与接收客户端与文件服务器之间的描述字，关闭该文件服务器，线程分离，释放资源
  
      ``` c++
      send_file_thread.detach();
      ```

## 2. chat_client
未完待续......





