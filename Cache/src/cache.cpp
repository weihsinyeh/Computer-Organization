#include <bits/stdc++.h>
#include <fstream>
#include <stdlib.h> // exit();
#include <iomanip>
using namespace std;
typedef struct Block BLOCK;
typedef struct Node NODE;
typedef struct NodeFull NODEFULL;
int searchWayLRU(NODE* Algo[],BLOCK cache[][4],int index,int tag,int* numOfHit,int* numOfMiss);
int searchBlockLRU(NODEFULL** Algo,BLOCK cache[],int tag,int* numOfHit,int* numOfMiss,int blockNum);
int searchWayFIFO(NODE* Algo[],BLOCK cache[][4],int index,int tag,int* numOfHit,int* numOfMiss);
int searchBlockFIFO(NODEFULL** Algo,BLOCK cache[],int tag,int* numOfHit,int* numOfMiss,int blockNum);

struct Block{
	int tag;
	bool valid;
};
struct Node{
    NODE* back;
    NODE* front;
    int wayNum;
};
struct NodeFull{
    NODEFULL* back;
    NODEFULL* front;
    int blockNum;
};
int main(int argc, char* argv[]){
	srand(time(NULL));
	/*******file process*********/
    fstream FILEIN,FILEOUT;
	FILEIN.open(argv[1],ios::in);
	FILEOUT.open(argv[2],ios::out | ios::trunc);
	/*******file process*********/

	int cacheSize, blockSize, Asso, Replace, wayNum, wordAddr, numOfHit=0, numOfMiss=0;
	FILEIN >> cacheSize >> blockSize >> Asso >> Replace;

	if(Asso == 0) wayNum = 1 ;
	if(Asso == 1) wayNum = 4 ;
	if(Asso == 2) wayNum = 1;
	int blockNum = cacheSize/blockSize/wayNum;
	int c = -1;
    if(Asso == 0){ //Directed Mapped
    	BLOCK cache[blockNum];
	for(int i=0;i<blockNum;i++){
		cache[i].valid = false;
		cache[i].tag = -1;
	}
       	while(FILEIN>>wordAddr){
       		int blcokAddr = wordAddr/blockSize;
            int index = blcokAddr % blockNum;
           	int tag = blcokAddr/blockNum;
            if(cache[index].tag == tag && cache[index].valid == true){
           		FILEOUT << c <<endl;
              	numOfHit++;
            }
           	else{
           		if(cache[index].valid ==false ) FILEOUT<< c <<endl;
            	else           		            FILEOUT<<cache[index].tag<<endl;
            		cache[index].valid = true;
            		cache[index].tag = tag;
                	numOfMiss++;
            }
        }
    }
    else if(Asso == 1){ //4-way set associative

    	BLOCK cache[blockNum][4]; //其實這邊的blockNum 應該為setNumber 
		NODE* Algo[blockNum];

    	for(int i=0;i<blockNum;i++){
   			for(int j=0;j<wayNum;j++){
   				cache[i][j].valid = false;
       			cache[i][j].tag = 0;
			}
    	}

    	for(int i=0;i<blockNum;i++){
    		NODE* first = (NODE*) malloc(sizeof(NODE));
        	(*first).wayNum = 0;
        	NODE* pre = first;
        	Algo[i] = first;
        	for(int j=1;j<4;j++){
        	   	NODE* newNODE = (NODE*) malloc(sizeof(NODE));
        	   	(*newNODE).wayNum = j;
        	   	(*newNODE).front = pre;
        	   	(*pre).back = newNODE;
        	   	pre = newNODE;
        	}
        	(*first).front = pre;
        	(*pre).back = first;
		}
    	if(Replace == 1) //LRU
    		while(FILEIN>>wordAddr){
    			int blcokAddr = wordAddr/blockSize;
            	int index = blcokAddr % blockNum;
				int tag =  blcokAddr/blockNum;
				FILEOUT<<searchWayLRU(Algo,cache,index,tag,&numOfHit,&numOfMiss)<<endl;
    		}

    	if(Replace == 0) //FIFO
    		while(FILEIN>>wordAddr){
    			int blcokAddr = wordAddr/blockSize;
            	int index = blcokAddr % blockNum;
				int tag =  blcokAddr/blockNum;
				FILEOUT<<searchWayFIFO(Algo,cache,index,tag,&numOfHit,&numOfMiss)<<endl;
    		}

	}
	else if(Asso == 2 ){ //full-associative
		BLOCK cache[blockNum];
		NODEFULL* Algo;

    	for(int i=0;i<blockNum;i++){
   			cache[i].valid = false;
       		cache[i].tag = 0;
    	}
    	for(int i=0;i<1;i++){
    		NODEFULL* first = (NODEFULL*) malloc(sizeof(NODEFULL));
        	(*first).blockNum = 0;
        	NODEFULL* pre = first;
        	Algo = first;
        	for(int j=1;j<blockNum;j++){
        		NODEFULL* newNODE = (NODEFULL*) malloc(sizeof(NODEFULL));
        	    (*newNODE).blockNum = j;
        	    (*newNODE).front = pre;
        	    (*pre).back = newNODE;
        	    pre = newNODE;
        	}
        	(*first).front = pre;
        	(*pre).back = first;
		}
    	if(Replace == 1)  //LRU
			while(FILEIN>>wordAddr){
				int tag = wordAddr/blockSize;
				FILEOUT<<searchBlockLRU(&Algo,cache,tag,&numOfHit,&numOfMiss,blockNum)<<endl;
			}

		if(Replace ==0)  //FIFO
			while(FILEIN>>wordAddr){
				int tag = wordAddr/blockSize;
				FILEOUT<<searchBlockFIFO(&Algo,cache,tag,&numOfHit,&numOfMiss,blockNum)<<endl;
			}
	}
	FILEOUT.precision(6);
	FILEOUT<<"Miss rate = "<<std::fixed<<setprecision(6)<<(double)numOfMiss/(double)(numOfHit + numOfMiss)<<endl;
   	FILEOUT.close();
}
int searchBlockLRU(NODEFULL** Algo,BLOCK cache[],int tag,int* numOfHit,int* numOfMiss,int blockNum){
    NODEFULL* curr = *Algo;
    int goal = -1,emptyNum =0;
    for(int i=0;i<blockNum;i++){
        if(cache[(*curr).blockNum].tag == 0)  emptyNum++;
        if(cache[(*curr).blockNum].tag == tag){ //exist //update
            goal = (*curr).blockNum;
            //exist but is least recently use
            if((*(*Algo)).blockNum == goal)   *Algo = (*(*Algo)).front;
            //exist but is not least recently use
            else{
                NODEFULL* originNew = (*(*Algo)).back;
                if((*originNew).blockNum != (*curr).blockNum){
                	//connect current's front and back
                	(*((*curr).back)).front = (*curr).front;
                	(*((*curr).front)).back = (*curr).back;
                	//update new
                	(*(*Algo)).back = curr;
                	(*curr).front = *Algo;
                	(*curr).back = originNew;
                	(*originNew).front = curr;
                }
            }
            break;
        }
        curr = (*curr).back;
    }
    int temp;
    if(goal == -1 && emptyNum==0){
        int victim = cache[(*(*Algo)).blockNum].tag;
        cache[(*(*Algo)).blockNum].valid = true;
        cache[(*(*Algo)).blockNum].tag = tag;
        *Algo = (*(*Algo)).front;
        (*numOfMiss)++;
        temp = victim;
    }
    if(goal == -1 && emptyNum!=0){
        cache[(*(*Algo)).blockNum].tag = tag;
        cache[(*(*Algo)).blockNum].valid = true;
        *Algo = (*(*Algo)).front;
        (*numOfMiss)++;
        temp = -1;
    }
    if(goal != -1){
        (*numOfHit)++;
        temp = -1; //not replacement;
    }
    return temp;
}
int searchWayLRU(NODE* Algo[],BLOCK cache[][4],int index,int tag,int* numOfHit,int* numOfMiss){
    NODE* curr = Algo[index];
    int goal = -1,emptyNum =0;
    for(int i=0;i<4;i++){
        if(cache[index][(*curr).wayNum].tag == 0) emptyNum++;
        if(cache[index][(*curr).wayNum].tag == tag){ //exist update
            goal = (*curr).wayNum;
            //exist but is least recently use
            if((*(Algo[index])).wayNum == goal) Algo[index] = (*(Algo[index])).front;
            //exist but is not least recently use
            else{
                NODE* originNew = (*(Algo[index])).back;
                if((*originNew).wayNum != (*curr).wayNum){
                	//connect current's front and back
                	(*((*curr).back)).front = (*curr).front;
                	(*((*curr).front)).back = (*curr).back;
                	//update new
                	(*(Algo[index])).back = curr;
                	(*curr).front = Algo[index];
                	(*curr).back = originNew;
                	(*originNew).front = curr;
                }
            }
            break;
        }
        curr = (*curr).back;
    }
    int temp;
    if(goal == -1 && emptyNum==0){
        int victim = cache[index][(*(Algo[index])).wayNum].tag;
        cache[index][(*Algo[index]).wayNum].valid = true;
        cache[index][(*Algo[index]).wayNum].tag = tag;
        Algo[index] = (*(Algo[index])).front;
        (*numOfMiss)++;
        temp = victim;
    }
    if(goal == -1 && emptyNum!=0){
        cache[index][(*Algo[index]).wayNum].tag = tag;
        cache[index][(*Algo[index]).wayNum].valid = true;
        Algo[index] = (*(Algo[index])).front;
        (*numOfMiss)++;
        temp = -1;
    }
    if(goal != -1){
        (*numOfHit)++;
        temp = -1; //not replacement;
    }
    return temp;
}

int searchWayFIFO(NODE* Algo[],BLOCK cache[][4],int index,int tag,int* numOfHit,int* numOfMiss){
    NODE* curr = Algo[index];
    int goal = -1,emptyNum =0;
    for(int i=0;i<4;i++){
        if(cache[index][(*curr).wayNum].tag == 0) emptyNum++;

        if(cache[index][(*curr).wayNum].tag == tag){ //exist
            goal = (*curr).wayNum;
            break;
        }
        curr = (*curr).back;
    }
    int temp;
    if(goal == -1 && emptyNum==0){
        int victim = cache[index][(*(Algo[index])).wayNum].tag;
        cache[index][(*Algo[index]).wayNum].valid = true;
        cache[index][(*Algo[index]).wayNum].tag = tag;
        Algo[index] = (*Algo[index]).back;
        (*numOfMiss)++;
        temp = victim;
    }
    if(goal == -1 && emptyNum!=0){
        cache[index][(*Algo[index]).wayNum].tag = tag;
        cache[index][(*Algo[index]).wayNum].valid = true;
        Algo[index] = (*Algo[index]).back;
        (*numOfMiss)++;
        temp = -1;
    }
    if(goal != -1){
    	(*numOfHit)++;
		 temp = -1; //not replacement;
	}
    return temp;
}
int searchBlockFIFO(NODEFULL** Algo,BLOCK cache[],int tag,int* numOfHit,int* numOfMiss,int blockNum){
    NODEFULL* curr = *Algo;
    int goal = -1,emptyNum =0;
    for(int i=0;i<blockNum;i++){
        if(cache[(*curr).blockNum].tag == 0) emptyNum++;

        if(cache[(*curr).blockNum].tag == tag){ //exist
            goal = (*curr).blockNum;
            break;
        }
        curr = (*curr).back;
    }
    int temp;
    if(goal == -1 && emptyNum==0){
        int victim = cache[(*(*Algo)).blockNum].tag;
        cache[(*(*Algo)).blockNum].valid = true;
        cache[(*(*Algo)).blockNum].tag = tag;
        *Algo = (*(*Algo)).back;
        (*numOfMiss)++;
        temp = victim;
    }
    if(goal == -1 && emptyNum!=0){
        cache[(*(*Algo)).blockNum].tag = tag;
        cache[(*(*Algo)).blockNum].valid = true;
        *Algo = (*(*Algo)).back;
        (*numOfMiss)++;
        temp = -1;
    }
    if(goal != -1) {
    	(*numOfHit)++;
		temp = -1; //not replacement;
	}
        return temp;
}
