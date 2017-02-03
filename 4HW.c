#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

#define thc 8

int counter[thc][128]= {0};
int partition[thc+1] = {0};
char buffer[65536];
void *runner(void *param);

int main( int argc, char *argv[] ){
	
	char *src;
	int fsrc,size,fsize;
	src=argv[1];
	
	if(argc!=2){
		printf("Invalid Arguments, argc is %d\n",argc); // if there aren't 2 additional args, its invalid
	}else{
		fsrc=open(src,O_RDONLY); //open source file for read only
		if(fsrc!=-1){ //if the file opens correctly...
			do{
				size=read(fsrc, buffer, 65536); //read to buffer
				if(size>0){ //if read is succsess...
					for(int i=0; i<thc;i++){
						partition[i] = i*(size/thc);
					}
					partition[thc]=size;
					pthread_t workers[thc]; //create thread array
					pthread_attr_t attr;
					pthread_attr_init(&attr); //instantiate attributes
					
					for(int i=0;i<thc;i++){//create threads to start at runner
						pthread_create(&workers[i],&attr,runner,(void*)(i+1)); 
					}
					
					for(int i=0;i< thc;i++){ //join all threads together
						pthread_join(workers[i],NULL);
					}



					fsize+=size; //increment file size
				}
			}while(size>0); //repeat as long as there is more to count
			
			//PRINT OUT RESULTS
			int charcount=0;
			int sum=0;
			for(int i=0;i<128;i++){
				sum=0;
				for(int j=0; j<thc; j++){
					sum+= counter[j][i];
				}
				if(i<32){ //print non character ascii vals
					printf("%d occurrences of 0x:%d\n",sum,i);
				}else{//print character vals
					printf("%d occurrences of '%c'\n",sum,i);
				}
				counter[0][i]=sum;
				charcount+=sum;
			}
			printf("counted %d characters.\n",charcount);
		}else{
			printf("Invalid Files, fsrc is %d \n",fsrc); //if files were invalid, report as such
		}
		close(fsrc);//close files
	}	
	
	return 0;
}
void *runner(void *param){
	//get bounds and read buffer segment
	for(int i=partition[(int)param-1];i<=partition[(int)param]-1;i++){ 
		counter[(int)param-1][(int)buffer[i]]++;
	}
	pthread_exit(0);
}