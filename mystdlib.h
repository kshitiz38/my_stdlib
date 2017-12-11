#include <sys/types.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    int cnt;
    char *ptr;
    char *base;
    int flag;
    int eof;
    int fd;
} MyFILE;

#define _READ 01
#define _WRITE 02
#define _EOF 010

#define NULL 0
#define EOF -1

#define _MAX_OPEN 20
#define _PERMS 0666
#define _BUFSIZ 4096

extern MyFILE _iob[_MAX_OPEN];

MyFILE *Myfopen(const char *filename, const char *mode );
int Myfclose(MyFILE *stream);
size_t Myfread(void *buf, size_t size, size_t nobj, MyFILE *stream);
size_t Myfwrite(const void *buf, size_t size, size_t nobj, MyFILE *stream);
int Myfseek(MyFILE *stream, long offset, int origin);
int Myputc(char c, MyFILE *stream);
int Mygetc(MyFILE *stream);
