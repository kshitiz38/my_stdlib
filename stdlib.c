#include <stdio.h>
#include <time.h>
#define _EOF -1
int main(){
    char line[100] = "The Quick Brown Fox Jumped Over The Lazy Dog";
    clock_t start;
    clock_t end;
    float elapsed;


    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nOPEN\n");
    start = clock();
    FILE *fp = fopen("write_test.txt", "w");
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Opened: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nCLOSE\n");
    start = clock();
    fclose(fp);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Closed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nWRITE\n");
    start = clock();
    FILE *fp1 = fopen("write_test.txt", "w");
    fwrite(line, strlen(line)+1, 1, fp1);
    fclose(fp1);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nREAD\n");
    start = clock();
    FILE *fp2 = fopen("write_test.txt", "r");
    char read_buf[100];
    fread(read_buf, strlen(line)+1, 1, fp2);
    fclose(fp2);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Content Read from File: %s || ", read_buf);
    printf("File Read Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nGETC\n");
    start = clock();
    FILE *fp4 = fopen("write_test.txt", "r");
    int i;
    for(i=0; i<strlen(line); i++){
        int tmp = getc(fp4);
        if(tmp!=_EOF){
            printf("%c", (char)tmp);
        } else {
            printf("\n");
            break;
        }

    }
    printf("\n");
    fclose(fp4);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Read Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");


    printf("\nPUTC\n");
    start = clock();
    FILE *fp3 = fopen("write_test.txt", "a");
    int j;
    for(j=0; j<strlen(line); j++){
        putc(line[j], fp3);
    }
    fclose(fp3);
    end = clock();
    elapsed = (float)(end - start) / CLOCKS_PER_SEC;
    printf("Char-Wise File Write Completed: Time Taken %fs\n", elapsed);
    printf("###############################\n");



    return 0;
}
