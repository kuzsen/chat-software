#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <json-c/json.h>


int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	server_addr.sin_addr.s_addr = inet_addr("172.17.7.99");
	//向服务器发起连接
	int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (-1 == ret)
	{
		perror("connect");
		exit(1);
	}

	//char buf[320] = "{'cmd':'register','user':'小明','密码':'11111'};";
	struct json_object* obj = json_object_new_object();
	json_object_object_add(obj, "cmd", json_object_new_string("offline"));
	json_object_object_add(obj, "user", json_object_new_string("小李"));
	json_object_object_add(obj, "password", json_object_new_string("11111"));

	const char* buf = json_object_to_json_string(obj);

	ret = send(sockfd, buf, strlen(buf), 0);
	if (-1 == ret)
	{
		perror("send");
		exit(1);
	}

	char s[128] = { 0 };
	ret = recv(sockfd, s, sizeof(s), 0);
	printf("收到服务器回复 %s\n", s);

	while (1);

	close(sockfd);

	return 0;
}