#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

#ifdef DEBUG
#define PDEBUG 1
#else
#define PDEBUG 0
#endif

#define DPRINT(fmt, ...) do { if (PDEBUG) fprintf(stderr, "%s:%d:%s(): "fmt,\
		__FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
		


void operation1(char* in,char *out)
{
    out[0] = (in[0] | in[1]) & (in[2] | in[3]);
    out[1] = in[2];
    out[2] = in[1];
    out[3] = in[1] & in[2] & in[3];
}
unsigned int operation2(char *in)
{
    unsigned int res = 0;
    res = res | (in[1] << 24);
    res = res | (in[3] << 16);
    res = res | (in[2] << 8);
    res = res | in[0];
    return res;
}

int main()
{
    int pipes[2][2];
    for(int i=0;i<2;i++)
    {
        if(pipe(pipes[i])<0)
        {
            DPRINT("Failed to Create Pipes");
            exit(0);
        }
    }
    int pid1=fork();
     if(pid1<0)
    {
        DPRINT("Failed to create Pipe");
        exit(0);
    }
    if(pid1==0)
    {
        int mypid = getpid();
        close(pipes[1][0]);
        close(pipes[0][1]);
        DPRINT("child(%d): starting\n", mypid);
        char result;
        int count=0;
        char arr[4];
        while((read(pipes[0][0],&result,1))!=EOF)
        {
            if(result=='\0')
            {
                arr[count]='z';
                count++;
                if(count==4)
                {
                    fprintf(stderr,"%c%c%c%c\n",arr[0],arr[1],arr[2],arr[3]);
                    char temp[4];
                    operation1(arr,temp);
                    write(pipes[1][1],&temp,4);
                    break;
                }
            }
            else
            {
                arr[count]=result;
                count++;
                if(count==4)
                {
                    fprintf(stderr,"%c%c%c%c\n",arr[0],arr[1],arr[2],arr[3]);
                    char temp[4];
                    operation1(arr,temp);
                    write(pipes[1][1],&temp,4);
                    count=0;
                }

            }
        }
        write(pipes[1][1],"\n\n\n\n",4);
        close(pipes[0][0]);
        close(pipes[1][1]);
        DPRINT("child(%d): ending\n",mypid);
        exit(0);
    }
    int pid2=fork();
    if(pid2<0)
    {
        DPRINT("Failed to create child 2 procress");
        exit(0);
    }
    if(pid2==0)
    {
        close(pipes[0][0]);
        close(pipes[0][1]);
        close(pipes[1][1]);
        char temp[4];
        unsigned int sum=0;
        while((read(pipes[1][0],&temp,4))!=EOF)
        {
            if(strcmp(temp,"\n\n\n\n")==0)
            {
                break;
            }
            unsigned int ir=operation2(temp);
            fprintf(stderr,"ir:%u\n",ir);
            sum+=ir%4294967296;
        }
        fprintf(stderr,"Final Sum:%u\n",sum);
        exit(0);
    }
    close(pipes[1][0]);
    close(pipes[1][1]);
    close(pipes[0][0]);
    FILE *FIN=fdopen(0,"r");
    int result;
    while((result=getc(FIN))!=EOF)
    {
        write(pipes[0][1],&result,1);
    }
    write(pipes[0][1],"\0",1);
    fclose(FIN);
    close(pipes[0][1]);
    waitpid(pid1,NULL,0);
    waitpid(pid2,NULL,0);
    return 0;
}
