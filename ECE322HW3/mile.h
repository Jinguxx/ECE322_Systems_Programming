#ifndef _MILE_H_
#define _MILE_H_

typedef	struct MILE{

    int _fd; //file descriptor
    char _mode; //r/w  (opperend is special write)
    void *_buf; //buffer 
    /*
    it is any piece of memory where data is stored temporarily 
    until it is processed or copied to the final destination 
    (or other buffer)
    */
    int buflen;//capacity of buffer
    int rp;//read position
    int wp;//write position
    int cp;//current position
    char *rwflag;

}MILE;

MILE *mopen(const char *path, const char *mode);
int mflush(MILE *stream, int i);
int mread(void *b, int i, MILE *stream);
int mwrite(void *b, int i, MILE *stream);
int mclose(MILE *stream);

#endif /*_MILE_H_*/