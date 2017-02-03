#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main( int argc, char *argv[] ){
	char *src, *dest, buffer[1024]; //Create File names and transfer buffer
	int counter=0;
	int fsrc,fdes,size,fsize;
	src=argv[1];
	dest=argv[2];
	
	if(argc!=3){
		printf("Invalid Arguments\n"); // if there aren't 2 additional args, its invalid
	}else{
		fsrc=open(src,O_RDONLY); //open source file for read only
		fdes=open(dest,O_CREAT|O_EXCL|O_WRONLY,S_IRWXU);
		//create dest file only if it doesn't exist.
		if(fsrc!=-1 && fdes!=-1){ //if the files open correctly...
			do{
				size=read(fsrc, buffer, 1024); //read to buffer
				if(size>0){ //if read is succsess...
					write(fdes,buffer,size); //write to buffer
					fsize+=size; //increment file size
				}
			}while(size>0); //repeat as long as there is more to copy
		}else{
			printf("Invalid Files\n"); //if files were invalid, report as such
		}
		close(fsrc);//close files
		close(fdes);
		printf("Copied %d bytes.\n",fsize);//report file transfer size
	}
	return 0;
}