#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "mile.h"

#define BUFSIZE 100

MILE *mopen(const char *path, const char *mode){ //path also stands for file
/*
mode_of_operation: This is also of C string type and refers to the mode of the file access. Below are the file access modes for C: 
“r” – Searches file. Opens the file for reading only. If the file is opened successfully fopen() loads it into memory and sets up a pointer which points to the first character in it. If the file cannot be opened fopen() returns NULL.
“w” – Searches file. If the file exists already, its contents are overwritten. If the file doesn’t exist, a new file is created. Returns NULL, if unable to open the file. It creates a new file for writing only(no reading).
“a” – Searches file. If the file is opened successfully fopen() loads it into memory and sets up a pointer that points to the last character in it. If the file doesn’t exist, a new file is created. Returns NULL, if unable to open the file. The file is opened only for appending(writing at end of file).
*/

    MILE *newMILE = (MILE *)malloc(sizeof(MILE));
    //making space for MILE
    newMILE->buflen=BUFSIZE;
    newMILE->_buf = malloc(BUFSIZE);

    //initialize everything to zero
	newMILE->rp = 0;
	newMILE->wp = 0;
	newMILE->cp = 0;
    memset(newMILE->_buf,'\0',BUFSIZE);

    if(strcmp(mode,"r")==0){
        newMILE->_fd = open(path, O_RDONLY); //read only
		newMILE->_mode = 'r';
    }
    if(strcmp(mode,"w")==0){
        newMILE->_fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0600);
		newMILE->_mode = 'w';
    }
   if(strcmp(mode,"a")==0){
        newMILE->_fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0600);
		newMILE->_mode = 'a';
    }
    if(newMILE->_fd == -1){
        //when it fails to open file
        return NULL;
    }
    else{
        //Upon successful completion, fopen() shall return a pointer to the object controlling the stream. Otherwise, a null pointer shall be returned, [CX] [Option Start]  and errno shall be set to indicate the error. [Option End]
        return newMILE;
    }
}

int mflush(MILE *stream, int i){
/* 
ostream points to an output stream or an update stream in which the 
most recent operation was not input, the fflush function causes any 
unwritten data for that stream to be delivered to the host environment 
to be written to the file; otherwise, the behavior is undefined.
--used to clear the buffer and accept the next string flush(stdin);
*/
    int flushing = write(stream->_fd, stream->_buf,i);//?
    return flushing;
}

int mread(void *b, int i, MILE *stream){
/*
• read() returns the number of bytes read from the file on success and –1 on error
• if end of file is reached before reading any bytes, 0 is returned
• reading starts at the current file offset
• successful reads increase the current file offset (advance the file pointer) by the return value
• return value may be less than nbytes
• readingfromaregularfilethatdoesnothavenbytes from current file offset to end of file
• readingfromterminaldevicesisonelineatatime
• reading from record-oriented devices, e.g., magnetic tapes, is one record at a time
• readingfromanetworkisdependentonbuffering, congestion, and other factors
*/
if(i>stream->buflen){
    memset(stream->_buf,'\0',stream->buflen);
    stream->cp=read(stream->_fd,stream->_buf,stream->buflen);
    memcpy(b,stream->_buf,stream->buflen);
    stream->cp=stream->cp+mread(b+stream->cp,i-stream->buflen,stream);
}
else
{
    memset(stream->_buf,'\0',stream->buflen);
    if(stream->cp==0)
    {
        stream->cp=read(stream->_fd,stream->_buf,i);
        stream->rp=stream->rp +i;
        memcpy(b,stream->_buf,i);
    }
    else
    {
        stream->cp=read(stream->_fd,stream->_buf,i);
        memcpy(b,stream->_buf,i);
        stream->rp=stream->rp +i;
    }
}
return stream->cp;
}

int mwrite(void *b,int i, MILE *stream){
/*
• write() returns the number of bytes written into the file on success and –1 on error
• writing starts at the current file offset
• successful writes increase the current file offset (advance the file pointer) by the return value
*/

    if(i > stream->buflen){
        memset(stream->_buf,'\0',stream->buflen);
        memcpy(stream->_buf, b, stream->buflen);
        stream->wp = write(stream->_fd,stream->_buf,stream->buflen);
        stream->wp = stream->wp + mflush(stream, strlen(b)-stream->buflen);

    }
    else{
        memset(stream->_buf,'\0',stream->buflen);
        memcpy(stream->_buf, b, i);
        stream->wp = write(stream->_fd, stream->_buf,i);
    }

    return stream->wp;
}

int mclose(MILE *stream){
//After every successful file operations, you must always close a file. For closing a file, you have to use fclose function
/*
• close() returns 0 on success and –1 on error
• filedes is released on success back to the UNIX operating system
• all open files are closed by the kernel when their process is terminated
*/
    int closed = close(stream->_fd);
    stream->cp=0;
    stream->rp=0;
    stream->wp=0;
    memset(stream->_buf,'\0',stream->buflen);
    free(stream->_buf);
    memset(stream,'\0',sizeof(MILE));
    free(stream);
    return closed;
}