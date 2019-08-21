#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rbcp.h"//SiTCP官方工具改，用于配置开发板

#define TEST_PORT 24
#define SERVER_IP "192.168.10.16"


#ifndef ENTRY
#define ENTRY

int main(int argC, char* arg[])
{

  char pszVerb[] = "wrb";
  char pszArg1[] = "0xab";
  char pszArg2[] = "0x34";
  char ipAddr[] = "127.0.0.1";
  unsigned int rbcpPort = 4660;
/*
  [pszVerb] [psvArg1] [psvArg2]
  wrb [address] [byte_data] : Write byte
  wrs [address] [short_data]: Write short(16bit)
  wrw [address] [word_data] : Write word(32bit)
  rd [address] [length]     : Read data
  load [file name]          : Execute a script（此时svArg2为NULLPTR）
  quit                      : quit from this program
 */

  configBoard(ipAddr, rbcpPort, pszVerb, pszArg1, pszArg2);//配置开发板,推荐用文件配置


  struct sockaddr_in addr;
  int sockfd, len = 0;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  char buffer[65535];


  ///here must be PF_INET
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  /* 填写sockaddr_in*/
  bzero(&addr, sizeof(addr));
  addr.sin_family = PF_INET;
  addr.sin_port = htons(TEST_PORT);
  addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr))<0){
    printf("connection error \n");
    return 0; 
  }
  printf("connection OK\n");
  len = recv(sockfd,buffer,sizeof(buffer),0);
  printf("%s \n",buffer); //print server information
  
  
  while(1) {

    bzero(buffer, sizeof(buffer));

    printf("Please enter a string to send to server: \n");
    
    /* 从标准输入设备取得字符串*/
    //len = read(STDIN_FILENO, buffer, sizeof(buffer));

    /* 将字符串传送给server端*/
    sendto(sockfd, buffer, len, 0, (struct sockaddr *)&addr, addr_len);

    return 0;
    
    
    /* 接收server端返回的字符串*/
    
    len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);
    //len = recv(sockfd, buffer, sizeof(buffer),0);
    printf("Receive from server: %s\n", buffer);
    

  }

  
  close(sockfd);
  
  
  return 0;
}

// ----------------------------------------------------------------------------
// End of udp_client.c
#endif