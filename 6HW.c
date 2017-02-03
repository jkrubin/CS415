#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct frame{
	int value;
	int LRUnum;
};
//MAKE PAGE TABLES
struct frame *inversePageTable;
int *pageTable;


int pageNum;
int addrCount;
int missCount;
int maxPage;
int frameNum;
int i;
int addrCount;
int missCount;
int findMin;

int findReplace(){
	findMin=0;
	for(i=0;i<frameNum;i++){
		if(inversePageTable[i].LRUnum < inversePageTable[findMin].LRUnum){
			findMin=i;
		}
	}
	return findMin;
}
/*
void LRU(int nextPage){
	if(pageTable[nextPage]==-1){
		findReplace();
		pageTable[(inversePageTable[findMin].value)]=-1;
		pageTable[nextPage]=findMin;
		inversePageTable[findMin].value=nextPage;
		inversePageTable[findMin].LRUnum=addrCount;
		printf("put page %d in frame %d\n",nextPage,pageTable[nextPage]);
	}
	else{
		printf("page %d hit\n",nextPage);
		inversePageTable[pageTable[nextPage]].LRUnum=addrCount;
	}
}

void FIFO(int nextPage){
	if(pageTable[nextPage]==-1){
		findReplace();
		pageTable[(inversePageTable[findMin].value)]=-1;
		pageTable[nextPage]=findMin;
		inversePageTable[findMin].value=nextPage;
		inversePageTable[findMin].LRUnum=addrCount;
		printf("put page %d in frame %d\n",nextPage,pageTable[nextPage]);
	}
	else{
		printf("page %d hit\n",nextPage);
	}
}
*/

int main( int argc, char *argv[] ){
	char buff[8];  
	unsigned int addr;
	unsigned int pageSize;
	unsigned int memSize;
	
	//Validate args
	if (argc !=5){
		printf("uses 4 args: log of page size, logical memory size, replacement type, and allocated frames");
		return -1;
	}
	//Init page specifications
	pageSize= pow(2,atoi(argv[1]));
	memSize= pow(2,atoi(argv[2]));
	frameNum= atoi(argv[4]);
	maxPage=memSize/pageSize;
	inversePageTable=malloc(frameNum*sizeof(struct frame));
	pageTable = malloc(maxPage * sizeof(int));
	for(i=0;i<maxPage;i++){
		pageTable[i]=-1;
	}
	for(i=0;i<frameNum;i++){
		inversePageTable[i].value=-1;
		inversePageTable[i].LRUnum=-1;
	}
	//Initialize counters
	addrCount=1;
	missCount=0;
	do{
		//Get input number
		//printf("\nInput a hex number: ");
		fgets(buff,8,stdin); 	//get the logical memory address
		
		//See if it is ending string
		if(strcmp(buff,"\n")!=0){//if not end string...
		
			//get page number from next input address
			addr = strtoul(buff,NULL,16);
			pageNum= (int)addr/pageSize;
			//printf("page num:%d\n",pageNum);
			//check if mem address is too big
			if(pageNum>maxPage){
				printf("Page fault: memory location is out of bounds");
				return -1;
			}
			//Check if its a miss...
			if(pageTable[pageNum]==-1){
				//update miss count
				missCount++;
				//Find entry with lowest replace value
				findReplace();
				//get the page to be replaced, set its val to -1 on pagetable
				pageTable[(inversePageTable[findMin].value)]=-1;
				//set new page to the frame that has been replaced
				pageTable[pageNum]=findMin;
				//update inverse page table
				inversePageTable[findMin].value=pageNum;
				//both FIFO and LRU use LRUnum, but LRU updates it on a hit...
				//FIFO does not
				inversePageTable[findMin].LRUnum=addrCount;
				//printf("put page %d in frame %d\n",pageNum,pageTable[pageNum]);
			}
			else{//its a hit
				//printf("page %d hit\n",pageNum);
				if(strcmp(argv[3],"LRU")==0){
					//update LRU if we have LRU type
					inversePageTable[pageTable[pageNum]].LRUnum=addrCount;
				}else if(strcmp(argv[3],"FIFO")==0){
				}else{
					printf("invalid replacement type, accepts \"LRU\" and \"FIFO\"");
					return -1;
				}
			}	
					

			addrCount++;
					
		}
	}while(strcmp(buff,"\n")!=0);
	printf("miss count is %d\n",missCount);
	free(inversePageTable);
	free(pageTable);
	return 0;
}