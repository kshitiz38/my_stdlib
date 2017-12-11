#include "mystdlib.h"

MyFILE _iob[_MAX_OPEN];

int Myvalidstream(MyFILE *stream){
    int j, valid=0;
    for(j=0; j<_MAX_OPEN; j++){
        if(_iob[j].fd==stream->fd){
            valid=1;
            break;
        }
    }
    if (!valid){
        printf("Invalid File Stream given as input\n");
        return -1;
    }
    return 1;
}

MyFILE *Myfopen(const char *filename, const char *mode ){
    MyFILE *stream;
    int fd;

    for(stream=_iob; stream < _iob + _MAX_OPEN; stream++){
        if((stream->flag & (_READ | _WRITE)) == 0){
            break;
        }
    }
    if (stream >= _iob + _MAX_OPEN){
        printf("Error: No Free Slots available\n");
        return NULL;
    }

    int FLAGS;
    if (strcmp(mode, "r")==0){
        FLAGS = O_RDONLY;
        fd = open(filename, FLAGS, 0);
    } else if (strcmp(mode, "w")==0){
        FLAGS = O_WRONLY;
        fd = creat(filename, _PERMS);
    } else if (strcmp(mode, "a")==0){
        FLAGS = O_WRONLY;
        fd = open(filename, FLAGS, 0);
        if (fd==-1){
            fd = creat(filename, _PERMS);
        }
        lseek(fd, 0L, 2);
    } else {
        printf("Error: Incoorect File Mode\n");
        return NULL;
    }

    if (fd==-1){
        if (FLAGS==O_RDONLY){
            printf("Error: Cannot Read File %s\n", filename);
            return NULL;
        } else if (FLAGS==O_WRONLY){
            printf("Error: Can't Create File %s, with Mode %03o\n", filename, _PERMS);
            return NULL;
        }
    }

    stream->fd = fd;
    stream->cnt = 0;
    stream->base = NULL;
    stream->ptr = NULL;
    stream->eof = NULL;
    stream->flag = strcmp(mode,"r") ? _WRITE : _READ;
    return stream;
}

int Myfclose(MyFILE *stream){
    if (Myvalidstream(stream)<0)
        return NULL;
    if (stream->base != NULL) {
        if (stream->flag & _WRITE) {
            if (write(stream->fd, stream->base, stream->cnt)!=stream->cnt){
                printf("Error: IO failure\n");
                return NULL;
            } else {
                stream->cnt = 0;
                stream->ptr = stream->base;
            }
        } else if (stream->flag & _READ) {
            stream->cnt = 0;
            stream->ptr = stream->base;
        }
    }

    close(stream->fd);
    stream->fd = NULL;
    stream->cnt = 0;
    stream->flag = NULL;
    stream->eof = NULL;
    if(stream->base)
        free(stream->base);
    if(stream->ptr)
        stream->ptr = NULL;
    return 0;
}

size_t Myfread(void *buf, size_t size, size_t nobj, MyFILE *stream){
    if (Myvalidstream(stream)<0)
        return NULL;

    if (stream->flag != _READ){
        printf("Incorrect File Access Mode...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    size_t bytes_requested = size * nobj;
    size_t bytes_read;

    if (bytes_requested==0) {
        return 0;
    }
    bytes_read = read(stream->fd, (char *) buf, bytes_requested);
    if (bytes_read==-1) {
        printf("Error: IO failure...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    return bytes_requested==bytes_read ? nobj:(bytes_read/size);
}

size_t Myfwrite(const void *buf, size_t size, size_t nobj, MyFILE *stream){
    if (Myvalidstream(stream)<0)
        return NULL;

    if (stream->flag != _WRITE){
        printf("Incorrect File Access Mode...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    size_t bytes_requested = size * nobj;
    size_t bytes_written;

    if (bytes_requested==0) {
        return 0;
    }
    bytes_written = write(stream->fd, (const char *)buf, bytes_requested);
    if (bytes_written==-1) {
        printf("Error: IO failure...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    return bytes_requested==bytes_written ? nobj:(bytes_written/size);
}

int Myfseek(MyFILE *stream, long offset, int origin){
    if (Myvalidstream(stream)<0)
        return NULL;

    if (stream->base != NULL) {
        if (stream->flag & _WRITE) {
            if (write(stream->fd, stream->base, stream->cnt)!=stream->cnt){
                printf("Error: IO failure...Closing File\n");
                Myfclose(stream);
                return NULL;
            } else {
                stream->cnt = 0;
                stream->ptr = stream->base;
            }
        } else if (stream->flag & _READ) {
            stream->cnt = 0;
            stream->ptr = stream->base;
        }
    }
    if (lseek(stream->fd, offset, origin) >= 0) {
        return 1;
    } else {
        printf("Error: IO failure...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
}

int Myputc(char c, MyFILE *stream){
    if (Myvalidstream(stream)<0)
        return NULL;

    if (stream->flag!=_WRITE){
        printf("Incorrect File Access Mode...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    if(!stream->base){
        stream->base = malloc(_BUFSIZ*sizeof(char));
        stream->ptr = stream->base;
    } else {
        if(stream->cnt==_BUFSIZ && stream->eof==NULL){
            if (write(stream->fd, stream->base, _BUFSIZ)!=_BUFSIZ){
                printf("Error: IO failure...Closing File\n");
                Myfclose(stream);
                return NULL;
            } else {
                stream->cnt = 0;
                stream->ptr = stream->base;
            }
        } else if(stream->eof==_EOF){
            if (write(stream->fd, stream->base, _BUFSIZ)!=_BUFSIZ){
                printf("Error: IO failure...Closing File\n");
                Myfclose(stream);
                return NULL;
            } else {
                stream->cnt = 0;
                stream->ptr = stream->base;
            }
            return 1;
        }
    }
    *stream->ptr++ = c;
    stream->cnt++;
    return 1;
}

int Mygetc(MyFILE *stream){
    if (Myvalidstream(stream)<0)
        return NULL;

    if (stream->flag!=_READ){
        printf("Incorrect File Access Mode...Closing File\n");
        Myfclose(stream);
        return NULL;
    }
    if(stream->cnt==0 && stream->eof==NULL){
        if(!stream->base)
            stream->base = malloc(_BUFSIZ*sizeof(char));
        stream->cnt = read(stream->fd, stream->base, _BUFSIZ);
        if (stream->cnt!=_BUFSIZ){
            stream->eof = _EOF;
        }
        stream->ptr = stream->base;
    } else if (stream->cnt==0 && stream->eof==_EOF){
        return EOF;
    }

    if (stream->cnt-- > 0){
        return (unsigned char) *stream->ptr++;
    }
}

int main(){
    char line[100] = "The Quick Brown Fox Jumped Over The Lazy Dog\n";
    clock_t start;
    clock_t end;
    float elapsed;


    printf("\nOPEN\n");
    start = clock();
    MyFILE *fp = Myfopen("write_test.txt", "w");
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Opened: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nCLOSE\n");
    start = clock();
    Myfclose(fp);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Closed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nWRITE\n");
    start = clock();
    MyFILE *fp1 = Myfopen("write_test.txt", "w");
    Myfwrite(line, strlen(line)+1, 1, fp1);
    Myfclose(fp1);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nREAD\n");
    start = clock();
    MyFILE *fp2 = Myfopen("write_test.txt", "r");
    char *read_buf = malloc(100*sizeof(char));
    Myfread(read_buf, strlen(line)+1, 1, fp2);
    Myfclose(fp2);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Content Read from File: %s", read_buf);
    printf("File Read Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nGETC\n");
    start = clock();
    MyFILE *fp4 = Myfopen("write_test.txt", "r");
    int i;
    for(i=0; i<strlen(line)+1; i++){
        int tmp = Mygetc(fp4);
        if(tmp!=_EOF){
            printf("%c", (char)tmp);
        } else {
            break;
        }

    }
    Myfclose(fp4);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Read Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");

    printf("\nGETC - Sys Calls\n");
    start = clock();
    MyFILE *fp6 = Myfopen("write_test.txt", "r");
    int j;
    char tmp;
    for(j=0; j<strlen(line); j++){
        int n = read(fp6->fd, &tmp, 1);
        printf("%c", tmp);
    }
    Myfclose(fp6);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Read Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nPUTC\n");
    start = clock();
    MyFILE *fp3 = Myfopen("write_test.txt", "a");
    for(j=0; j<strlen(line); j++){
        Myputc(line[j], fp3);
    }
    Myfclose(fp3);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nPUTC - Sys Calls\n");
    start = clock();
    MyFILE *fp5 = Myfopen("write_test.txt", "a");
    for(j=0; j<strlen(line); j++){
        int n = write(fp5->fd, &line[j], 1);
    }
    Myfclose(fp5);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");

    return 0;
}
