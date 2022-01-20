#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef DEBUG
#define PDEBUG 1
#else
#define PDEBUG 0
#endif

#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
		__FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
		


char *oper1(char in[]){
	/*This operation works with char data in blocks of 4. This operation 
	mixes an array of four chars into a new array of 4 chars using the operations shown below.
	Input: char in[4] Output: char out[4] Operation:
	out[0] = (in[0] | in[1]) & (in[2] | in[3]) out[1] = in[2]
	out[2] = in[1]
	out[3] = in[1] & in[2] & in[3]
	*/
    char *out = malloc(sizeof(char)*4);
	out[0] = (in[0] | in[1]) & (in[2] | in[3]);
	out[1] = in[2];
	out[2] = in[1];
	out[3] = in[1] & in[2] & in[3];

    return out;
}

unsigned int oper2(char in[]){

	/*This operation works with unsigned 32-bit integers. This operation 
	converts an array of 4 chars into an unsigned integer.
	Input: char in[4] Output: unsigned int res Operation:
	res = 0
	res = res | (in[1] << 24) res = res | (in[3] << 16) res = res | (in[2] << 8) res = res | in[0]
	*/
	unsigned int res = 0;
	res = res | (in[1] << 24);
	res = res | (in[3] << 16);
	res = res | (in[2] << 8);
	res = res | in[0];	
	
	return res;	
}

void runChild1(int pipe1[], int pipe2[]){

    char *out = malloc(sizeof(char)*4);
    char result;
    int count = 0;
    char arr[4];
    
    FILE *f0 = fdopen(pipe1[0],"r");

    while(fscanf(f0,"%c",&result)!=EOF) {
        if(count < 4){
            arr[count] = result;
            count++;
        }
        else{
            out = oper1(arr);
            for(int j = 0; j < 4; j++){
            printf("%c", arr[j]);
            write(pipe2[1], (void*)&out[j],1);
        }
        printf("\n");
        count = 0;
        arr[count] = result;
        count++;
    }
    fclose(f0); //close here before the writing the Zs
    if(count<4 && count!=0){
        for(int j = count; j < 4; j++){
            //printf("you are a z");
            arr[j] = 'z';
            //i++;
        }
    }
    char temp[4];
    out = oper1(arr);
    for(int j = 0; j < 4; j++){
        printf("%c", arr[j]);
        write(pipe2[1], (void*)&out[j],1);
    }
    printf("\n");
    close(pipe2[1]);
    }
}

void runChild2(int pipe2[]){

    FILE *f0 = fdopen(pipe2[0], "r");
    unsigned int finalResult = 0;
    unsigned int initialResult = 0;
    char infoFromPipe;
    char arr[4];
    int i = 0;
    while(fscanf(f0, "%c", &infoFromPipe) != EOF){
    //printf("%c\n", infoFromPipe);
        if(i < 4){
            arr[i] = infoFromPipe;
            i++;
        }
        else{
            initialResult = oper2(arr);
            printf("ir: %u \n", initialResult);
            finalResult = finalResult + initialResult;
            //printf("initial: %u \n \n", initialResult);
            i = 0;
            arr[i] = infoFromPipe;
            i++;
        }
    }
    if(i == 4){
        initialResult = oper2(arr);
        finalResult = finalResult + initialResult;
        printf("ir: %u \n", initialResult);
    }
    printf("final result: %u \n\n", finalResult);
    fclose(f0);
    exit(0);
}

int main(int argc, char *argv[]) {

	int pid1;
    int pid2;

	DPRINT("parent start\n");
	DPRINT("parent: starting child\n");

    int pipe1[2];//array of 2 file descriptors for pipe 1
    int pipe2[2];//array of 2 file descriptors

	pipe(pipe1);//create pipe
	pipe(pipe2);//create pipe

//-----------------------------------------------------------------------------------------------
	//Child 1

    pid1 = fork();
    if(pid1<0)
    {
        DPRINT("Failed to create Pipe\n");
        exit(0);
    }
	if (pid1==0) { // child 1 
		int mypid = getpid();
		DPRINT("child(%d): started\n",mypid);

		close(pipe1[1]); //close pipe 1 write
        close(pipe2[0]);

        DPRINT("child(%d): reading from pipe 1\n",mypid);
        runChild1(pipe1,pipe2);

        close(pipe1[0]);
        close(pipe2[1]);//close pipe 2
		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}
//-----------------------------------------------------------------------------------------------
    //parent code
	int mypid = getpid();
    DPRINT("parent(%d):continue\n",mypid);
    DPRINT("parent(%d):wait for child(%d)\n",mypid,pid1);

    close(pipe1[0]);
    close(pipe2[1]);

    char userInput;
    printf("Give your input: ");
    
    while(scanf("%c",&userInput)!=EOF){
        write(pipe1[1],(void *)&userInput,1);
    }

    close(pipe1[1]);

    DPRINT("parent(%d):waiting for child to finish\n",mypid);
    waitpid(pid1, NULL, 0); // wait for child1 to finish
    //waitpid(pid2, NULL, 0); // wait for child1 to finish

    DPRINT("parent(%d): end\n",mypid);
   
  
//-----------------------------------------------------------------------------------------------
	//Child 2
    pid2 = fork();
    if(pid2<0)
    {
        DPRINT("Failed to create Pipe\n");
        exit(0);
    }
    if (pid2==0) { // child code 2
		int mypid = getpid();
		DPRINT("child(%d): started\n", mypid);
		
        close(pipe2[1]);//close the write end of pipe2
        close(pipe1[0]);
        close(pipe1[1]);
      
        runChild2(pipe2);
        
        exit(0);

		DPRINT("child(%d): end\n",mypid);
	}

//-----------------------------------------------------------------------------------------------
   

    return 0;
}