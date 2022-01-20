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
		
/*
Parameters :
fd[0] will be the fd(file descriptor) for the 
read end of pipe.
fd[1] will be the fd for the write end of pipe.

Returns : 0 on Success.
-1 on error.
*/



void oper1(char *in, char*out){
	/*This operation works with char data in blocks of 4. This operation 
	mixes an array of four chars into a new array of 4 chars using the operations shown below.
	Input: char in[4] Output: char out[4] Operation:
	out[0] = (in[0] | in[1]) & (in[2] | in[3]) out[1] = in[2]
	out[2] = in[1]
	out[3] = in[1] & in[2] & in[3]
	*/

	out[0] = (in[0] | in[1]) & (in[2] | in[3]);
	out[1] = in[2];
	out[2] = in[1];
	out[3] = in[1] & in[2] & in[3];
}

unsigned oper2(char *in){

	/*This operation works with unsigned 32-bit integers. This operation 
	converts an array of 4 chars into an unsigned integer.
	Input: char in[4] Output: unsigned int res Operation:
	res = 0
	res = res | (in[1] << 24) res = res | (in[3] << 16) res = res | (in[2] << 8) res = res | in[0]
	*/
	unsigned int res = 0;
	res = 0;
	res = res | (in[1] << 24);
	res = res | (in[3] << 16);
	res = res | (in[2] << 8);
	res = res | in[0];	
	
	return res;	
}

int main() {

	int pid;

	DPRINT("parent start\n");
	DPRINT("parent: starting child\n");

    int pipe1[2];//array of 2 file descriptors for pipe 1
	pipe(pipe1);//create pipe
	int pipe2[2];//array of 2 file descriptors
	pipe(pipe2);//create pipe

	if(pipe(pipe1) && pipe(pipe2) < 0){
		perror("Failed to Create Pipes");
		exit(0);
	}

//-----------------------------------------------------------------------------------------------
	//Child 1

	/*Child1 reads data from Pipe1, processes it, and writes it to Pipe2 
	a. When Child1 receives an EOF from Pipe1:
		i. It closes Pipe1
		ii. Writes remaining data to Pipe2
		iii. Closes Pipe2
		iv. Exits
	*/

	/*1. Reads data from Pipe1 until EOF 
		a. For every 4 chars it gets:
			i. Writes input chars to stdout 
			ii. Carries out Oper1
			iii. Writes output to Pipe2 
	  2. When EOF
		a. Closes Pipe1
		b. If it has not received a full set of 4 chars, it fills in the remaining spaces with the
			letter 'z'
			i. Carries out Oper1
			ii. Writes output to Pipe2 
		c. Closes Pipe2
		d. Exits
	*/

	pid = fork();
	if(pid<0)
    {
        DPRINT("Failed to create child 1 process\n");
        exit(0);
    }
	if (pid==0) { // child 1 
		int mypid = getpid();
		DPRINT("child(%d): started\n",mypid);

		close(pipe1[1]); //close pipe 1 write
		close(pipe2[0]); //close pipe 2 read

		char result;
		int count = 0;
		char arr[4];
		
		DPRINT("child(%d): reading from pipe 1\n",mypid);
		//runChild1(pipe1, pipe2);
		
		while((read(pipe1[0], &result,1)) != EOF){

			if(result=='\0'){//when it has not recieved a full set of 4 chars, fills with 'z'
				arr[count]='z';
				count++;
				if(count == 4){
					printf("%c%c%c%c\n",arr[0],arr[1],arr[2],arr[3]);
					char temp[4];
					oper1(arr,temp);
					DPRINT("child(%d): writing to pipe2\n",mypid);
					write(pipe2[1],&temp,4);
					break;
				}
			}
			else{//reads from pipe1, every 4 chars writes to stdout, op1, and writes to pipe 2
				arr[count]=result;
                count++;
                if(count==4){
                    printf("%c%c%c%c\n",arr[0],arr[1],arr[2],arr[3]);
                    char temp[4];
                    oper1(arr,temp);
					DPRINT("child(%d): writing to pipe 2\n",mypid);
                    write(pipe2[1],&temp,4);
                    count=0;
                }
			}
		}
		
		write(pipe2[1],"\n\n\n\n",4);
        close(pipe1[0]);//close pipe 1
        close(pipe2[1]);//close pipe 2
		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
	//Child 2

	/*Child2 reads data from Pipe2, processes it, and writes to stdout
	a. When Child2 receives an EOF from Pipe2: 
		i. It closes Pipe2
		ii. Does final processing of received data 
		iii. Writes final result 
			to stdout
		iv. Exits
	*/

	/*1. Initializes final result to 0
	2. Reads data from Pipe2 until EOF
		a. For every 4 chars it gets: 
			i. Carries out Oper2
			ii. Writes intermediate result to stdout 
			iii. Adds intermediate result to final result
	3. When EOF
		a. Closes Pipe2
		b. Writes final result to stdout c. Exits

	*/
	
	if (pid==0) { // child code 2
		int mypid = getpid();
		DPRINT("child(%d): started\n", mypid);
		
		close(pipe1[0]);
        close(pipe1[1]);//close the write end of pipe1
        close(pipe2[1]);//close the write end of pipe2

		char temp[4];
		unsigned int sum = 0; //initialize final result to 0

		DPRINT("child(%d): reading from pipe 2\n",mypid);
		while((read(pipe2[0], &temp,4)) != EOF){
			if(strcmp(temp,"\n\n\n\n")==0)
            {
                break;
            }
            unsigned int ir = oper2(temp);
            printf("ir:%u\n",ir);
            sum+=ir%4294967296;
        }
        printf("final result:%u",sum);
		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}

//-----------------------------------------------------------------------------------------------
	//parent
	int mypid = getpid();
	DPRINT("parent(%d):reading in from stdin and writing to pipe 1\n",mypid);

	close(pipe1[0]);// close read end pipe 1
	close(pipe2[0]);//close read end of pipe 2
	close(pipe2[1]);

	// connect to stdio
	FILE *FIN = fopen("test.txt","r");

	int result;
    while((result=getc(FIN))!=EOF){
        write(pipe1[1],&result,1);
    }
	write(pipe1[1],"\0",1);

	fclose(FIN);

	close(pipe1[1]);//close write end of pipe 1
	//close(pipe2[1]);//close write end of pipe 2

	DPRINT("parent(%d):wait for child\n",mypid);

	waitpid(pid, NULL, 0); // wait for child1 to finish

	DPRINT("parent(%d):end\n",mypid);

    return 0;
}