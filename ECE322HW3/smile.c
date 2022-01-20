#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mile.h"
#include "smile.h" 

int mgetc(struct MILE *stream){
/*
fgetc() is used to obtain input from a file single character at a time. 
This function returns the ASCII code of the character read by the function. 
It returns the character present at position indicated by file pointer. 
After reading the character, the file pointer is advanced to next character. 
If pointer is at end of file or if an error occurs EOF file is returned by this function. 
*/

    char ch;
	int intint = mread(&ch, 1, stream);
	//checks to see if its readable
	if(intint > 0){
		return ch;
	}
	else
		return 0;
}

int mputc(int ch, struct MILE*stream){
/*   
putc() is used to write a single character at a time to a given file. 
It writes the given character at the position denoted by the file pointer and then advances 
the file pointer. This function returns the character that is written in case of 
successful write operation else in case of error EOF is returned. 
*/

    mwrite(&ch, 1, stream);
    return 0;
}

int mungetc(int ch, struct MILE *stream){
    /*
    The ungetc() function takes a single character and shoves it back onto an input stream. 
    It is the opposite of the getc() function, which reads a single character from an input stream.
    Also, ungetc() is an input function, not an output function.
    */

    int intint = mread(&ch, 1, stream);
	if(intint > 0){
		stream->rp = stream->rp - 1;
		return ch;
	}
	else
		return 0;
}

int mread_int(struct MILE *stream){
 
    char* buf;
    int size = 1;
    buf = malloc(size);
    char temp;
    int result;
    
    while((temp = mgetc(stream)) !=10){
        strncat(buf, &temp, 1);
        buf = realloc(buf,size+1);
    }
    result=atoi(buf); //change to chars
    return result;

}

int mwrite_int(int i,struct MILE *stream){

    int len = i;
    int integer = 0;
    int index = 0;

    if(i < 0){
        mputc('-',stream);
        integer++;
        i=-i;
    }
    while(len){
        integer++;
        len/=10;
    }
    char arr[integer];
    while(i){
        arr[++index] = i%10+'0';
        i/=10;
    }
    for(int j = 0; j < index; j++){
        mputc(arr[index-j],stream);
    }
    return integer;

}