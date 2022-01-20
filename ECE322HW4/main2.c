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

int readData(char *fname, char **arr, const int len) {
	char line[LLEN];
	int count = 0;
	FILE *fin = fopen(fname,"r");
	if (fin) {
		memset((void *)line, '\0', LLEN);
		while ((count < len) && (fscanf(fin,"%s",line)!=EOF)) {
			arr[count] = strdup(line);
			memset((void *)line, '\0', LLEN);
			count++;
		}
	}
	fclose(fin);
	return count;
}

void printData(char **arr, const int len) {
	for(int i=0;i<len;i++) printf("%s\n",arr[i]);
}

void bsort(char **arr, const int len) {
	for(int i=0;i<len;i++) {
		for(int j=0;j<len-i-1;j++) {
			if (strcmp(arr[j],arr[j+1])>0) {
				char *tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
			}
		}
	}
}

void runChildCode(char *fname) {

	DPRINT("child starting\n");

	char **data;
	data = (char **)malloc(sizeof(char *)*ALEN);
	DPRINT("child reading data\n");
	int len = readData(fname,data,ALEN);
	DPRINT("child read %d lines\n",len);

	DPRINT("child print data\n");
	printData(data,10);

	DPRINT("child sort data\n");
	bsort(data,len);

	DPRINT("child print data\n");
	printData(data,10);
	
	for(int i=0;i<len;i++) free(data[i]);
	free(data);
	DPRINT("child done\n");
}


int main() {

	int pid;

	DPRINT("parent start\n");
	
	DPRINT("parent: starting child\n");
	pid = fork();
	if (pid==0) { // child code
		int mypid = getpid();
		DPRINT("child(%d): started\n",mypid);

		runChildCode("d1.dat");

		DPRINT("child(%d): end\n",mypid);
		exit(0);
	} 
	// parent code
	int mypid = getpid();
	DPRINT("parent(%d):continue\n",mypid);
	DPRINT("parent(%d):wait for child(%d)\n",mypid,pid);

	waitpid(pid, NULL, 0); // wait for child to finish


	pid = fork();
	if (pid==0) { // child code
		int mypid = getpid();
		DPRINT("child(%d): started\n",mypid);

		runChildCode("d2.dat");

		DPRINT("child(%d): end\n",mypid);
		exit(0);
	} 

	// parent code

	waitpid(pid, NULL, 0); // wait for child to finish

	DPRINT("parent(%d): end\n",mypid);


	return 0;
}

