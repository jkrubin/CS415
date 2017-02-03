#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

	//Make sure parent process waits for child process to complete
	//	-POSIX: Wait(chid||null(waits for all children)) 
	//	-WIN32: WaitForSingleObject(*FIRST,Infinite) 
	//		-FIRST = CreateProcess(process_information ) struct
	char buff[1024]; //instantiate vars
	char *arg1;
	char *arg2;
	char *arg3;
	pid_t child;
	do{
		printf("MyShell>> ");
		fgets(buff,1024,stdin); //get input
		strtok(buff,"\n");		//chop off new line
	
		arg1 = strtok(buff," ");
		arg2 = strtok(NULL," "); //parse through buff
		arg3 = strtok(NULL," ");
		
		printf("0:%s\n1:%s\n",arg1,arg2);
		if(strcmp(arg1,"exit")!=0){ //Make sure its not exit command 
			if(arg2==NULL){ //if there is only one arg
				if((child = fork()) == 0){ //fork and ID child 
					execlp(arg1,arg1,NULL); //child executes command 
				}else{ //parent only 
					wait(0);
				}
			}else if(arg3==NULL){ //if there are 2 args
				if((child = fork()) == 0){ //fork and ID child
					execlp(arg1,arg1,arg2,NULL); //child executes 2 arg command 
				}else{ //parent only
					wait(0);
				}
			}else{//if there are 2+ args(but we only accept 3)
				if((child = fork()) == 0){ //fork and ID child 
					execlp(arg1,arg1,arg2,arg3,NULL); //child executes 3 arg command 
				}else{ //parent only
					wait(0);
				}
			}
		}
	}while(strcmp(arg1,"exit")!=0);//keep shell operating until exit command 
	return 0;
}