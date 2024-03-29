//
// Created by 97252 on 6/13/2022.
//

#ifndef UNIX_FILE_SYSTEM_MYFILE_H
#define UNIX_FILE_SYSTEM_MYFILE_H
#include "stdio.h"
#include "ufs.h"

enum mode{A, W, R, R_P};

typedef struct myFILE {
    char *data;
    mydirent *dir;
    int fd;
    int size;
    int seek_pos;
    int type;
    int mode;


} myFILE;


/** Methods */
myFILE *myfopen(const char * pathname, const char * mode);
int myfclose(myFILE *stream);
size_t myfread(void * ptr, size_t size, size_t nmemb, myFILE * stream);
size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream);
int myfseek(myFILE *stream, long offset, int whence);
int myfscanf(myFILE * stream, const char * format, ...);
int myfprintf(myFILE * stream, const char * format, ...);

#endif //UNIX_FILE_SYSTEM_MYFILE_H
