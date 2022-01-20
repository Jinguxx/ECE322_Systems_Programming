#ifndef _SMILE_H_
#define	_SMILE_H_

#include "mile.h"

int mgetc(MILE *stream);
int mputc(int ch,  MILE*stream);
int mungetc(int ch,  MILE *stream);
int mread_int( MILE *stream);
int mwrite_int(int i, MILE *stream);

#endif