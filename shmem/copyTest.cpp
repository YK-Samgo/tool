#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <signal.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include "mmap_ring.h"

using namespace std;

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


int main(int argC, char* argv[]){
	//buffer ,share memory between the ReadData() fork and the WriteMongoDB() fork
	unsigned char *bufferHead= new unsigned char [SIZE_128M];
	ringBuffer rb{bufferHead, SIZE_128M};
	//memory arrange:
	//

	//seperate the ReadData() fork and the WriteMongoDB() fork
	//int pid = fork();

	cout<<"fork.. " <<endl;
	unsigned int startDataTrans{0x0F0F0F0F};
	unsigned int stopSign{0xFF00FF00};
	int mainPID = (int)getpid();
	int pid = fork();

	if (pid < 0 )  {
    cout << "ERROR: Create the WriteMongoDB() fork failed! Exit." << endl;
    exit(1);
  }else if (pid != 0){ //parent fork, ReadData()
    	cout<<"I am in parent processs " <<endl;
    	int pieceSize = 1048576;
    	unsigned char* temp = new unsigned char[pieceSize];
		FILE *f = fopen("p-u-m.cia", "rb");
		if (f == NULL){
			exit(0);
		}
		int fileLength = getlength(f);
		printf("file size: %d\n", fileLength);
		int len{pieceSize};
		while (fileLength != 0)
		{
			len = pieceSize;
			if (fileLength > 0){
				len = std::min(pieceSize, fileLength);
			}
			fread(temp, len, 1, f);
			fileLength -= len;

    		while(1){
	  			if(rb.canWrite()>4+4+len){
	    			rb.write(&startDataTrans, 4); ///4bytes
	    			rb.write(&len, 4);
	    			rb.write(temp, len);
	    			break; 
	  			}else if(kill(pid, 0) != 0){
      				cout << "ERROR: the writemongodb() process failed! Exit" << endl;
      				exit(-1);
    			}
	  			usleep(1000);
			}
		}
		delete temp;
		fclose(f);
		rb.write(&stopSign, 4);
		return 0;
  }else{
  	    cout<<"I am in child processs " <<endl;
  	    unsigned int ifStart{0};
  	    int spareTime{0};//calculate the buffer spare time, in case that the parent fork exit by accident.(so that the parent fork not emit the stop sign.)
    	int bytesGet{0};
  		FILE *fp;
  		fp = fopen("fileRecv.dat","a");
  	    while (1){
  	      	if(rb.canRead() > 0){
  	        	rb.read(&ifStart, 4);
  				if (ifStart == startDataTrans){//when the data comes wrong, the process will throw away the broke data until it finds the correct sign
  		  			int buffersize;
  		  			rb.read(&buffersize, 4);
  		  			char* boardbuffer = new char[buffersize];
  		  			rb.read(boardbuffer, buffersize);

  		  			fwrite(boardbuffer, buffersize, 1, fp);
  		  			
  		  			bytesGet += buffersize;

  					spareTime = 0;
	  				delete boardbuffer;
				}else if(ifStart == stopSign){
					fclose(fp);
					cout << "child stop" << endl;
	  				return 0;
				}
      		}else{
      			spareTime += 1;
      			if(spareTime > 5){
      				if (kill(mainPID, 0) != 0){
            			cout << "daqMain: the readData() process failed! Exit" << endl;
      					exit(-1);
      				}
      				spareTime = 0;
      			}
				sleep(1);
  			}
  		}
  		
  	}
}