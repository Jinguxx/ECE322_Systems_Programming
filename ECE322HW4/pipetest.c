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

#define ALEN 10 //array length
#define LLEN 50 //line length


/*
Parameters :
fd[0] will be the fd(file descriptor) for the 
read end of pipe.
fd[1] will be the fd for the write end of pipe.

Returns : 0 on Success.
-1 on error.
*/

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

void write_to_file(char **str, char *outFile, int fileNum, int len){ 

	FILE *Ofile = fopen(outFile, "w"); //opening output file needed to write to it
	
	for(int i = 0; i < len; i++){
		//fprintf(Ofile,"%s\n",str[i]); //int fprintf(FILE *stream, const char *format, ...)
		fprintf(Ofile,"%d %s\n", fileNum, str[i]);
	}
	fclose(Ofile);
}

void mergeArray(char **a, int n1, char **b, int n2, char *fname)    //Function Definition
{
	char **mer;
	mer = (char **)malloc(sizeof(char *)*(n1+n2));
	int i, j, k, n3;
	j = k = 0;
	n3 = n1 + n2;	
	for(i = 0; i < n3;){
		if(j < n1 && k < n2){
			if(a[j] < b[k]){
				mer[i] = a[j];
				j++;
			}
			else{
				mer[i] = b[k];
				k++;
			}
			i++;
		}
		else if(j == n1){
			while(i < n3){
				mer[i] = b[k];
				k++;
				i++;
			}
		}
		else{
			while(i < n3){
				mer[i] = a[j];
				j++;
				i++;
			}
		}
	}
	write_to_file(mer, fname, 3, n3);
}

void runChildCodePipe3(char *fname, int pipe[]) {

	DPRINT("child starting for pipe 3\n");

	FILE *file = fdopen(pipe[0], "r");

	char **data1;
	data1 = (char **)malloc(sizeof(char *)*ALEN); //ALEN

	char **data2;
	data2 = (char **)malloc(sizeof(char *)*ALEN); //ALEN

	int wordCount1 = 0;
	int wordCount2 = 0;
	char wordLines[LLEN];
	int arrSize = ALEN;

	char fileHead[1];

	DPRINT("child reading data\n");

	//copy in data from the file into the pipe

	while(fscanf(file,"%s %s", fileHead, wordLines)!=EOF) {
		if(fileHead[0] == '1'){ //reallocate data
			if(wordCount1 == arrSize){
				arrSize = arrSize + 10;
				data1 = (char **)realloc(data1, sizeof(char *)*arrSize);
			}
			data1[wordCount1] = strdup(wordLines);
			wordCount1++;
		}
		else{
			if(wordCount2 == arrSize){
				arrSize = arrSize + 10;
				data2 = (char **)realloc(data2, sizeof(char *)*arrSize);
			}
			data2[wordCount2] = strdup(wordLines);
			wordCount2++;
		}
	}

	DPRINT("child read %d lines\n",wordCount1);
	DPRINT("child read %d lines\n",wordCount2);
	//printData(data1,20);
	//printData(data2,20);

	DPRINT("child read mergin arrays\n");
	mergeArray(data1, wordCount1, data2, wordCount2,fname);

}

void runChildCode(char *fname, int pipe[], int fileNum) {

	DPRINT("child starting\n");

	FILE *file = fdopen(pipe[0], "r"); 

	char **data;
	data = (char **)malloc(sizeof(char *)*ALEN);
	int wordCount = 0;
	char wordLines[LLEN];
	int arrSize = ALEN;

	DPRINT("child reading data\n");

	//copy in data from the file into the pipe
	while(fscanf(file,"%s",wordLines)!=EOF) {
		if(wordCount == arrSize){ //reallocate data
			arrSize = arrSize + 10;
			data = (char **)realloc(data, sizeof(char *)*arrSize);
		}
		data[wordCount] = strdup(wordLines); 
		wordCount++;
	}

	DPRINT("child read %d lines\n",wordCount);

	DPRINT("child sort data\n");
	bsort(data,wordCount);
	//printData(data,20);

	DPRINT("child write to file\n");
	write_to_file(data, fname, fileNum, wordCount);
	

	for(int i=0;i<wordCount;i++) free(data[i]);
	free(data);
	DPRINT("child done\n");
}

/*
// testing
void write_to_pipe (int file)
{
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf (stream, "hello\n");
  fprintf (stream, "goodbye\n");
  fprintf (stream, "monday\n");
  fprintf (stream, "apple\n");

  fclose (stream);
}
*/

void write_to_pipe(char *fname, int pipe1[]){

	FILE *file1 = fopen(fname, "r");
	FILE *f0 = fdopen(pipe1[1], "w");

	char str[20];
	int index = 0;

	while(fscanf(file1,"%s\n",str)!=EOF) {
		fprintf(f0, "%s\n", str);
	}

	fclose(file1);
	fclose(f0);

}

void write_two_to_pipe(char *fname1, char *fname2, int pipe[]){

	FILE *file1 = fopen(fname1, "r");
	FILE *file2 = fopen(fname2, "r");
	FILE *f1 = fdopen(pipe[1], "w");

	char str[20];
	int index = 0;

	while(fscanf(file1,"%s\n",str)!=EOF) {
		fprintf(f1, "%s\n", str);
	}
	while(fscanf(file2,"%s\n",str)!=EOF) {
		fprintf(f1, "%s\n", str);
	}

	fclose(file1);
	fclose(file2);
	fclose(f1);

}

int main() {

	int pid;

	DPRINT("parent start\n");
	DPRINT("parent: starting child\n");

	char **str = (char **)malloc(ALEN*sizeof(char *));
	int pipe1[2];//array of 2 file descriptors
	pipe(pipe1);//create pipe

//-----------------------------------------------------------------------------------------------
	//Child 1
	pid = fork();
	if (pid==0) { // child code 1
		int mypid = getpid(); //returns the process ID of the calling process. This is often used by routines that generate unique temporary filenames.
		DPRINT("child(%d): started\n",mypid);
		// close the write
		close(pipe1[1]);

		runChildCode("file1.txt", pipe1, 1);

		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}
//-----------------------------------------------------------------------------------------------

	// parent code
	int mypid = getpid();
	DPRINT("parent(%d):continue\n",mypid);
	DPRINT("parent(%d):wait for child(%d)\n",mypid,pid);
	// close read end
	close(pipe1[0]);

	// connect to stdio
	FILE *f1 = fdopen(pipe1[1],"w");

	//write(pipe1[1], string, (strlen(string)+1));
	DPRINT("parent(%d):continue, writing to pipe\n",mypid);
	write_to_pipe("d1.dat", pipe1);

	//fclose (f1);

	DPRINT("parent(%d):waiting for child to finish\n",mypid);

//-----------------------------------------------------------------------------------------------
	//Child 2
	//int pid2;
	int pipe2[2];//array of 2 file descriptors
	pipe(pipe2);//create pipe

	pid = fork();
	if (pid==0) { // child code 1
		int mypid = getpid(); //returns the process ID of the calling process. This is often used by routines that generate unique temporary filenames.
		DPRINT("child(%d): started\n",mypid);
		// close the write
		close(pipe2[1]);

		runChildCode("file2.txt", pipe2, 2);
		
		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}

//-----------------------------------------------------------------------------------------------
	// parent code
	DPRINT("parent(%d):continue\n",mypid);
	DPRINT("parent(%d):wait for child(%d)\n",mypid,pid);
	// close read end
	close(pipe2[0]);

	// connect to stdio
	FILE *f3 = fdopen(pipe2[1],"w");

	DPRINT("parent(%d):continue, writing to pipe\n",mypid);
	write_to_pipe("d2.dat", pipe2);

	//fclose (f3);


	close(pipe1[1]);
	close(pipe2[1]);

	DPRINT("parent(%d):waiting for child to finish\n",mypid);
	waitpid(pid, NULL, 0); // wait for child to finish
	

//-----------------------------------------------------------------------------------------------
	//Child 3
	int pipe3[2];//array of 2 file descriptors
	pipe(pipe3);//create pipe

	pid = fork();
	if (pid==0) { // child code 3	
	DPRINT("parent(%d):continue\n",mypid);

		int mypid = getpid(); //returns the process ID of the calling process. This is often used by routines that generate unique temporary filenames.
		DPRINT("child(%d): started\n",mypid);
		// close the write
		close(pipe1[1]);

		runChildCodePipe3("file3.txt", pipe3);

		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}

//-----------------------------------------------------------------------------------------------
	// parent code
	DPRINT("parent(%d):continue\n",mypid);
	DPRINT("parent(%d):wait for child(%d)\n",mypid,pid);
	// close read end
	close(pipe3[0]);

	// connect to stdio
	FILE *f4 = fdopen(pipe3[1], "w");

	DPRINT("parent(%d):writing to pipe\n",mypid);
	write_two_to_pipe("file1.txt", "file2.txt", pipe3);

	close(pipe3[0]);

	DPRINT("parent(%d):waiting for child to finish\n",mypid);
	waitpid(pid, NULL, 0); // wait for child to finish

	DPRINT("parent(%d): end\n",mypid);



	return 0;
}