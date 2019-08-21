#include "shmem_test.h"

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <signal.h>

unsigned int rww(char *p, int pos){
  //char s[4];
  //for(int k=0;k<4;k++){
  //s[k] = *(p+pos+k);
  //}
  unsigned int word{0};
  memcpy(&word, p+pos, 4);
  return word;
}


int main(int argC, char* argv[]) {
  char* parent_message = "helloabcdefghi";  // parent process will write this message
  char* child_message = "goodbyejklmnopqr"; // child process will then write this one

  unsigned char *bufferHead= new unsigned char [30];//The address of the head

  ringBuffer rb{bufferHead, 30, 3}; //create the shared memory with the ring buffer

  rb.write(parent_message, strlen(parent_message));
  unsigned char *flag1= new unsigned char [1];
  flag1[0] = 0x23;
  rb.writeFlags(flag1, 0);//put the flag1 into the first flag buffer, length 1 byte
  delete flag1;
  unsigned char *flag2= new unsigned char [2];
  flag2[0] = 0xFF;
  flag2[1] = 0x12;
  rb.writeFlags(flag2, 1, 2);//put the flag2 into the second flag buffer, length 2 bytes
  delete flag2;

  int mainPID = (int)getpid();
  printf("Main: main pid is %d\n", mainPID);
  int pid = fork();//create a new fork

  if (pid == 0) {
    int childPID = (int)getpid();
    printf("Child: child pid is %d\n", childPID);
    unsigned char *flag = new unsigned char [1];
    rb.readFlags(flag, 0, 1);
    printf("Child: read the first flag:0x%X\n", *flag);
    delete flag;
    
    char* message[40];
    printf("Child: Size of chars in the buffer now: %d\n", rb.canRead());
    rb.read(message, rb.canRead());
    printf("Child: read: %s\n", message);
    message[0] = "\0";
    printf("Child: Size of chars in the buffer now: %d\n", rb.canRead());
    rb.write(child_message, strlen(child_message));
    printf("Child: wrote: %s\n", child_message);
    printf("Child: Size of chars in the buffer after child writes: %d\n", rb.canRead());
    sleep(5);
    exit(0);
  } else {
    usleep(1000);
    int parentPID = (int)getpid();
    printf("Parent: parent pid is %d\n", parentPID);
    printf("Parent: child pid is %d\n", pid);
    unsigned char *flag = new unsigned char [2];
    rb.readFlags(flag, 1, 2);
    printf("Parent: read the second flag:0x%X%X\n", *(flag), *(flag + 1));
    delete flag;
    printf("Parent: if the child fork exist: %d\n", kill(pid, 0));
    char* message[40];
    sleep(1);
    printf("Parent: After 1s, size of chars in the buffer: %d\n", rb.canRead());
    rb.readOnly(message, rb.canRead());
    printf("Parent: data in the buffer: %s\n", message);
    message[0] = "\0";
    rb.readOnly(message, 6);
    printf("Parent: readOnly 6 chars: %s\n", message);
    message[0] = "\0";
    printf("Parent: size of chars in the buffer now: %d\n", rb.canRead());
    rb.read(message, 6);
    unsigned int word = rww((char *)message, 0);
    printf("Parent: rww test: %p\n", word);
    printf("Parent: read 6 chars: %s\n", message);
    message[0] = "\0";
    printf("Parent: size of chars in the buffer now: %d\n", rb.canRead());
    message[0] = "\0";
    int c = rb.read(message, rb.canRead());
    printf("Parent: if the child fork exist: %d\n", kill(pid, 0));
    printf("Parent: chars remaining %d bytes: %s\n", c, message);
    sleep(5);
    printf("Parent: After another 5 seconds, if the child fork exist: %d\n", kill(pid, 0));
    exit(0);
  }
}
