#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

int	pid1 = -5;
int pid2 = -5;
int	pipeID[2];
int	status;
char *args[10];
int waitSym;
int flag;
int file;

int isSym(char *str){
	if(strcmp(str,"<")==0){
		return 1;
	}else if(strcmp(str,">")==0){
		return 1;
	}else if(strcmp(str,"2>")==0){
		return 1;
	}else if(strcmp(str,">>")==0){
		return 1;
	}else if(strcmp(str,"2>>")==0){
		return 1;
	}else if(strcmp(str,"|")==0){
		return 1;
	}else if(strcmp(str,"&")==0){
		return 2;
	}
	return 0;
}

void exePipe(){

	if ((status = pipe(pipeID)) == -1) { /* error exit - bad pipe */
		perror("Bad pipe");
		exit(-1);
	}
	if ((pid1 = fork()) == -1) {  /* error exit - bad fork */
		perror("Bad fork");
		exit(-1);
	}
	if (pid1 == 0) { /* The first child process */
	/* make sure you start the last process in the pipeline first! */
		close (0);
		dup (pipeID[0]);
		close (pipeID[0]);
		close(pipeID[1]);
		execlp(args[5],args[5],args[6],args[7],args[8],NULL); 
		/* error exit - exec returned */
		perror("Execl returned");
		exit(-1);
	}
	/* this is the parent again */
	if ((pid2 = fork()) == -1) { /* error exit - bad fork */
		perror("Bad fork");
		exit(-1);
	}
	if (pid2 == 0) { /* the second child process */
		close (1);
		dup(pipeID[1]);
		close(pipeID[0]);close(pipeID[1]);
		execlp(args[0],args[0],args[1],args[2],args[3],NULL); 
		/* error exit - exec returned */
		perror("Execl returned");
		exit(-1);
	}
	/* back to the parent again */
	close(pipeID[0]);
	close(pipeID[1]);
	wait(&pid1);
	wait(&pid2);
	//printf("The parent is exiting\n");
}

void redirect(int dest){
	if(file == -1){
		fprintf(stderr, "Error in opening file\n");
		exit(0);
	}
	
	if ((pid1 = fork()) == -1) {  /* error exit - bad fork */
		perror("Bad fork");
		exit(-1);
	}
	if(pid1 == 0){
		//child process
		//Close the specified std Out, In or Err
		close(dest);
		//Dup file handle for redirect, filling the closed dest
		dup(file);
		//Close file, as it has replaced std in, out or err
		close(file);
		execlp(args[0],args[0],args[1],args[2],args[3],NULL); 
	}
	//Back to parent
	wait(&pid1);
	//printf("parent exiting\n");
}	

int main(){
	char buff[1024]; //instantiate vars
	pid_t child;
	int i;
	
	do{
		printf("HW7 shell>> ");
		fgets(buff,1024,stdin); //get input
		strtok(buff,"\n");//chop off new line
		args[0] = strtok(buff," ");
		i=1;
		flag=0;
		waitSym=0;
		/*
		/ Input Tokenized into 10 max args
		/ 0: Command for the first half of a pipe or redirect
		/	1: arg1 for first cmd OR redirect / pipe symbol OR NULL
		/	2: arg2 for first cmd OR redirect / pipe symbol OR NULL
		/	3: arg3 for first cmd OR redirect / pipe symbol OR NULL
		/ 4: redirect/pipe symbol 							OR NULL
		/ 5: Command after pipe symbol OR file name	 		OR NULL
		/ 	6: arg1 for second cmd 			-->				OR NULL
		/	7: arg2 for second cmd			--> 			OR NULL
		/	8: arg3 for second cmd			-->				OR NULL
		/	9: Extra args for second cmd	OR &			OR NULL
		/	10: Extra arg for second cmd 	OR &			OR NULL
		*/
		
		do{// GET THE FIRST COMMANDS UP UNTIL A SYMBOL OR MAX ARGS
			args[i] = strtok(NULL, " ");
			if(args[i]!=NULL){
				if(isSym(args[i])!=0){
					//Find the symbol and mark arg number
					if(isSym(args[i])==2){
						//if we find an & set to index in args
						waitSym=i;
					}else{
						//set flag to index of the symbol
						flag=i;
					}
				}
			}
			i++;
		}while(flag==0 && i<5); //Get up to 4 args and a symbol
		
		for(i=5;i<10;i++){ // get the last 5 args, they could be null
			args[i] = strtok(NULL, " ");
			if(args[i]!=NULL){
				if(isSym(args[i])==2){
					//Find the symbol and mark arg number
					waitSym=i;
				}
			}	
		}
		
		//print out all of the args for testing , even if they are null
		/*
		printf("waitsym is: %d\n",waitSym);
		for(i=0;i<10;i++){
			printf("%d:%s\n",i,args[i]);
		}
		*/
		if(strcmp(args[0],"exit")!=0){ //Make sure its not exit command 
			if(waitSym>0){ //If we have a wait symbol
				//Set arg to null because it isn't an arg, but we still recognize it's 
				//there with waitSym>0
				args[waitSym]=NULL;
				//printf("\nRecognized '&' symbol. implementation caused bugs...\n");
			}
// FIGURE OUT WHAT SYMBOL WE HAVE
			if(flag>0){//Check if we got a symbol
//	|	|	|	|	|	|	|	|	|	|	|
					if(strcmp(args[flag],"|")==0){ //Check if pipe symbol
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					exePipe(); //execute pipe	
// >	>	>	>	>	>	>	>	> 	>	>	
				}else if(strcmp(args[flag],">")==0){
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					//Open File to truncate existing
					file = open(args[5], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
					//redirect stdout to file
					redirect(1);
// 2>	2>	2>	2>	2>	2>	2>	2>	2>	2>	2>
				}else if(strcmp(args[flag],"2>")==0){
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					//Open to truncate existing
					file = open(args[5], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
					//redirect StdErr to file
					redirect(2);
// >>	>>	>>	>>	>>	>>	>>	>>	>>	>>	>>	>>
				}else if(strcmp(args[flag],">>")==0){
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					//Open file to append existing
					file = open(args[5], O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
					//Redirect stdout to file
					redirect(1);
// 2>>	2>>	2>>	2>>	2>>	2>>	2>>	2>>	2>>	2>>	2>>	2>>
				}else if(strcmp(args[flag],"2>>")==0){
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					//Open to append existing file
					file = open(args[5], O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
					//redirect StdErr to file
					redirect(2);
// <	<	<	<	<	<	<	<	<	<	<
				}else if(strcmp(args[flag],"<")==0){
					if(flag>0){
						args[flag]= NULL; // change a possible arg back to null
						//so we can do a uniform execlp
					}
					//Open to read input file
					file = open(args[5], O_RDONLY,NULL);
					//redirect file to StdIn
					redirect(0);
				}
			}else{//no extra symbol, execute as normal	
				if((child = fork()) == 0){ //fork and ID child 
					execlp(args[0],args[0],args[1],args[2],NULL); //child executes 3 arg command 
				}else{ //parent only
					//I am leaving out implementation of "&" because when i use it, it 
					//causes unexplainable bugs in my program. will talk about it in writeup
					if(waitSym==0){
						printf("waiting reg...\n");
						wait(&child);
					}
				}
			}
		}else{ // If args[0] is "exit" we exit
			exit(0);
		}
	
	}while(strcmp(args[0],"exit")!=0);//keep shell operating until exit command 
	return 0;
}