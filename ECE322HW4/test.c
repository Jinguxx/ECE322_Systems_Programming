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


int main() {

	int pid;

	DPRINT("parent start\n");
	
	DPRINT("parent: starting child\n");

	int pipe1[2];
	pipe(pipe1);

	pid = fork();
	if (pid==0) { // child code
		int mypid = getpid();
		DPRINT("child(%d): started\n",mypid);
		// close the write
		close(pipe1[1]);

		char str[20];
		int d;
		FILE *f0 = fdopen(pipe1[0],"r");

		while(fscanf(f0,"%d %s",&d,str)!=EOF) {
			DPRINT("child(%d): %d, %s\n",mypid,d,str);
		}

		fclose(f0);

		DPRINT("child(%d): end\n",mypid);
		exit(0);
	}
	// parent code
	int mypid = getpid();
	DPRINT("parent(%d):continue\n",mypid);
	// close read end
	close(pipe1[0]);

	// connect to stdio
	FILE *f1 = fdopen(pipe1[1],"w");

	fprintf(f1,"1 hello\n");
	fprintf(f1,"2 world\n");

	fclose(f1);

	waitpid(pid,NULL,0);

	DPRINT("parent(%d): end\n",mypid);


	return 0;
}