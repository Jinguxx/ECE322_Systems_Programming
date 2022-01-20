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
#define ALEN 10
#define LLEN 50
char *oper1(char in[]){
char *out = malloc(sizeof(char)*4);
out[0] = (in[0] | in[1]) & (in[2] | in[3]);
out[1] = in[2];
out[2] = in[1];
out[3] = in[1] & in[2] & in[3];
return out;
}
void child1Code(int pipe1[], int pipe2[]){
//child one will run oper1
char *out = malloc(sizeof(char)*4);
char infoFromPipe;
char arr[4];
int i = 0;
FILE *f0 = fdopen(pipe1[0],"r");
while(fscanf(f0,"%c",&infoFromPipe)!=EOF) {
// printf("%i : %c\n",i, infoFromPipe);
if(i < 4){
arr[i] = infoFromPipe;
i++;
}
else{
out = oper1(arr);
for(int j = 0; j < 4; j++){
printf("%c", arr[j]);
write(pipe2[1], (void*)&out[j],1);
}
printf("\n");
i = 0;
arr[i] = infoFromPipe;
i++;
}
}
fclose(f0); //close here before the writing the Zs
if(i<4 && i!=0){
for(int j = i; j < 4; j++){
//printf("you are a z");
arr[j] = 'z';
//i++;
}
}
out = oper1(arr);
for(int j = 0; j < 4; j++){
printf("%c", arr[j]);
write(pipe2[1], (void*)&out[j],1);
}
printf("\n");
close(pipe2[1]);
}
unsigned int oper2(char in[]){
unsigned int res = 0;
res = res | (in[1] << 24);
res = res | (in[3] << 16);
res = res | (in[2] << 8);
res = res | in[0];
return res;
}
void child2Code(int pipe2[]){
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
}
int main(int argc, char *argv[]){
//parent code
//parent code will create child1 and child2
DPRINT("parent start\n");
//parents:
//create pipe 1
int pipe1[2];
pipe(pipe1); //pipe1[0] is used for reading end and pipe1[1] is used for writing end
//create pipe 2
int pipe2[2];
pipe(pipe2);
DPRINT("parent: starting child\n");
int pid1;
pid1 = fork();
//create first child
if(pid1 == 0){
int mypid1 = getpid();
DPRINT("child(%d): started\n",mypid1);
close(pipe1[1]);
close(pipe2[0]);
child1Code(pipe1, pipe2);
close(pipe1[0]);
close(pipe2[1]);
DPRINT("child(%d): end\n",mypid1);
exit(0);
}else{
//continue the parent code
int mypid = getpid();
DPRINT("parent(%d):continue\n",mypid);
close(pipe1[0]);
close(pipe2[1]);
char userInput;
printf("Give your input: ");
while(scanf("%c",&userInput)!=EOF){
write(pipe1[1],(void *)&userInput,1);
}
// close write pipe to do EOF
close(pipe1[1]);
waitpid(pid1, NULL, 0);
DPRINT("parent(%d): end\n",mypid);
//*
}
int pid2;
pid2 = fork();
//create second child
if(pid2 == 0){
printf("Child2 Started\n");
close(pipe1[0]);
close(pipe1[1]);
close(pipe2[1]); //close write end
child2Code(pipe2);
close(pipe2[0]);
printf("Child2 End\n");
exit(0);
}
//*/
return 0;
}