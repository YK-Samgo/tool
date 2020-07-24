#include "mmap_ring.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <algorithm>

#define TARGET_PORT 24545
#define TARGET_IP "192.168.143.12"

int main(int argC, char* argv[])
{ 
  int test_size = SIZE_128M;
  unsigned char *buffer = new unsigned char [test_size];
  ringBuffer rb{buffer, test_size};

  int internetIn = fork();
  //int internetIn{0};

  if (internetIn < 0){
  	printf("Failed to open the fork: internetIn");
  	return -1;
  }else if (internetIn == 0){
  	struct sockaddr_in addr;
    int sockfd, len = 0;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char *rcvdData[65535];
    int bufferRest{65535};

    ///here must be PF_INET
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }

    char *targrtIP = (char*)std::string(TARGET_IP).c_str();
    uint16_t tragetPort = TARGET_PORT;
    /* 填写sockaddr_in*/
    if (argC == 3){
     targrtIP = argv[1];
     tragetPort = atoi(argv[2]);
   }

   bzero(&addr, sizeof(addr));
   addr.sin_family = PF_INET;
   addr.sin_port = htons(tragetPort);
   addr.sin_addr.s_addr = inet_addr(targrtIP);

   if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr))<0){
    printf("connection error \n");
    return 0; 
  }
  printf("connection OK\n");
    //const char* sendMessage = "client here";
    //sendto(sockfd, sendMessage, strlen(sendMessage), 0, (struct sockaddr *)&addr, addr_len);
    //len = recv(sockfd,rcvdData,sizeof(rcvdData),0);
    //printf("%s \n",rcvdData); //print server information

  /* 将字符串传送给server端*/
  send(sockfd, "start", 5, 0);
  while(1) {
    rcvdData[0] = "\0";
      //printf("Please enter a string to send to server: \n");

      /* 从标准输入设备取得字符串*/
      //len = read(STDIN_FILENO, rcvdData, sizeof(rcvdData));

      /* 将字符串传送给server端*/
      //sendto(sockfd, rcvdData, len, 0, (struct sockaddr *)&addr, addr_len);

      /* 接收server端返回的字符串*/
    bufferRest = std::min((unsigned int)(65535), rb.canWrite());
      //printf("Remaining: %dbytes\n", rb.canWrite());
      //len = recvfrom(sockfd, rcvdData, bufferRest, 0, (struct sockaddr *)&addr, &addr_len);
    len = recv(sockfd, rcvdData, bufferRest,0);
      //printf("Receive from server: %s\n", rcvdData);

    rb.write(rcvdData, len);
      //printf("InternetIn Cache used: %d\n", rb.canRead());
  }
  close(sockfd);
  }//end fork internetIn
  else{
  	int diskWrite = fork();

    if (diskWrite < 0){
     printf("Failed to open the fork : diskWrite");
     return -1;
   }else if(diskWrite == 0){
    unsigned char* temp = new unsigned char[1048576];
    int bytesGet{0};
    int bytesRead{0};
    int nevent{0};
    auto time0 = time(0);
    while (1){
      if (bytesRead = rb.read(temp, 1048576)){
        //FILE *fp;
        //fp = fopen("/mnt/z/fileRecv.dat","a");
        //fwrite(temp, bytesRead, 1, fp);
        //fclose(fp);
        bytesGet += bytesRead;
        nevent++;
        if (nevent % 1000 == 0){
          printf("Band Width: %1.2f\n GBps", double(bytesGet) / 1024 / 1024 / 1024 /difftime(time(0), time0));
          time0 = time(0);
          bytesGet = 0;
        }
      }else{
        printf("sleep\n");
        sleep(1);
      }
    }
    delete temp;
    return 0;
  }else{
    return 0;
  }
}

sleep(60000);
return 0;
}
