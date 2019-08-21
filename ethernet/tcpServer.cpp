#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#define SERVER_PORT 24545
#define SERVER_IP "127.0.0.1"

long getlength( FILE *fp )
{
	long cur_pos;
	long len;

 //取得当前文件流的读取位置
	cur_pos = ftell( fp );
 //将文件流的读取位置设为文件末尾
	fseek( fp, 0, SEEK_END );
 //获取文件末尾的读取位置,即文件大小
	len = ftell( fp );
 //将文件流的读取位置还原为原先的值
	fseek( fp, cur_pos, SEEK_SET );
	return len;
}

int main(int argC, char* argv[])
{
	struct sockaddr_in addr, clientAddr;
	int sockfd, len, nRet, linkFd = 0;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	socklen_t clientAddrLen;
	char *rcvdData[65500];
	int bufferRest{65535};

	char *serverIP = (char*)std::string(SERVER_IP).c_str();
	uint16_t serverPort = SERVER_PORT;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //接收所有客户端ip的连接
    addr.sin_port = htons(serverPort);//接收8080端口发来的连接

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket:");
		return -1;
	}

	/* 当TCP的连接的状态是TCP_WAIT状态的时候， 可以通过设置SO_REUSEADDR
        选项来强制使用属于TIME_WAIT状态的连接的socket*/
	int isReuse = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&isReuse,sizeof(isReuse));

    /* 将该socket的描述符和本地的套接字地址绑定起来 */
	nRet = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	if(-1 == nRet)
	{
		perror("bind:");
		return -1;
	}

    /* 设置该套接口在监听状态 */
	listen(sockfd ,1);

    /* 等待客户端发来的tcp连接 ,当客户端连接进来之后，返回两个之间的唯一的socket连接，存放在linkFd之中*/
	clientAddrLen = sizeof(struct sockaddr_in);
	linkFd = accept(sockfd,(struct sockaddr*)&clientAddr,&clientAddrLen);
	if(-1 == linkFd)
	{
		perror("accept:");
		return -1;
	}

    /* 把连接进来的客户端地址和端口打印出来 */
	printf("commect %s %d successful\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));

	while(1){
		rcvdData[0] = "\0";

      	/* 接收client端发送的字符串*/
      	//len = recvfrom(sockfd, rcvdData, bufferRest, 0, (struct sockaddr *)&addr, &addr_len);
		len = recv(linkFd, rcvdData, 65500,0);
		const char* command = (char *)rcvdData;
		unsigned char* temp = new unsigned char[65500];
		memset(temp, 0xa0, 65500);
		FILE *f = fopen(argv[1], "rb");
		if (f == NULL){
			exit(0);
		}
		int fileLength = getlength(f);
		printf("%d\n", fileLength);
		while (fileLength)
		{
			int len = std::min(65500, fileLength);
			fread(temp, len, 1, f);
			fileLength -= len;
			send(linkFd, temp, len, 0);
			usleep(1);
		}
		fclose(f);
		
	}
}