#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>

#define thc 8

int counter[128]= {0};
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
			int sum=0;
			for(int i=0;i<128;i++){
				if(i<32){ //print non character ascii vals
					//printf("%d occurrences of 0x:%d\n",counter[i],i);
				}else{//print character vals
					//printf("%d occurrences of '%c'\n",counter[i],i);
				}
				sum+= counter[i];
			}
			printf("RACE CONDITION\n\tcounted %d characters.\n",sum);
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
		counter[(int)buffer[i]]++;
		//printf("%d thread counting %c\n",(int)param,i);
	}
	pthread_exit(0);
}