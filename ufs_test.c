//
// Created by barak on 14/06/2022.
//

#include <stdio.h>
#include "stdlib.h"
#include "ufs.h"
#include "fcntl.h"

int main() {

    /** make new filesystem, open 2 files that do not exist with flag - O_CREAT
     *  and share the same directory that also do not exist with flag - O_CREAT
     *  the goal is to test if program manage to open a new file/directory when its not exist in the file system
     *
     *  after open these files, we write to the first file ("Hello world") and check the following steps:
     *              check if the pointer is in the right place, expected -> 11.
     *              use lseek to bring the pointer to the start expected -> 0.
     *              read the the first 5 chars of this opened file, expected -> "Hello"
     *
     *  closing the opened files
     *  saving the current filesystem changes to a desire location (int this case, the same location, for override check)
     *  destroying the filesystem using @destroy_mkmfs, to free all alocated memory in the program
     *  */

    mymkfs(10000);


    int ok = myopen("root/hello/OKOKOK", O_CREAT);
    int sababa = myopen("root/hello/SABABA", O_CREAT);
    char write_text[1024];
    strcpy(write_text, "Hello World");
    printf("size of write text is -> %d\n", strlen(write_text));



    int pointer = mywrite(ok, write_text, strlen(write_text));

    if(pointer == 11) {
        printf("~~~~~~~~~~ pointer-test Passed\n");
    } else {
        printf("~~~~~~~~~~ pointer-test Failed !!\n");
    }

    printf("curr pointer -> %d\n", pointer);
    int pos =mylseek(ok,0,SEEK_SET);

    if(pos == 0) {
        printf("~~~~~~~~~~ pointer-test Passed\n");
    } else {
        printf("~~~~~~~~~~ pointer-test Failed !!\n");
    }

    char read_text[1024];
    myread(ok, read_text, 5);

    if(!strcmp(read_text,"Hello")) {
        printf("~~~~~~~~~~ read-test Passed\n");
    } else {
        printf("~~~~~~~~~~ read-test Failed !!,\n ex -> Hello | actual -> %s\n",read_text);
    }

    myclose(ok);
    myclose(sababa);

    print_fs();
    save("my_file.txt");
    destroy_mkfs();




    /** load the previous saved file system and then try the methods.*/
    mymount(NULL, "my_file.txt", NULL, NULL, NULL);
    ok = myopen("root/hello/OKOKOK", O_CREAT);
    sababa = myopen("root/hello/SABABA", O_CREAT);

    memset(write_text,0, strlen(write_text));
    strcpy(write_text, "Hello World");

    printf("CHECK WHAT I WRITE %s\n", write_text);

    printf("size of write text is -> %ld\n", strlen(write_text));

    pointer = mywrite(ok, write_text, strlen(write_text));

    if(pointer == 11) {
        printf("~~~~~~~~~~ pointer-test Passed\n");
    } else {
        printf("~~~~~~~~~~ pointer-test Failed !!\n");
    }

    printf("curr pointer -> %d\n", pointer);
    pos = mylseek(ok,0,SEEK_SET);

    if(pos == 0) {
        printf("~~~~~~~~~~ pointer-test Passed\n");
    } else {
        printf("~~~~~~~~~~ pointer-test Failed !!\n");
    }

    memset(read_text,0, strlen(read_text));

    int nunbytes = myread(ok, read_text, 5);

    printf("HOW MANY i READ %d\n", nunbytes);

    if(!strcmp(read_text,"Hello")) {
        printf("~~~~~~~~~~~~~~~ read-test Passed\n");
    } else {
        printf("~~~~~~~~~~ read-test Failed !!,\n ex -> Hello | actual -> %s\n",read_text);
    }

    myclose(ok);
    myclose(sababa);

    print_fs();
    save("my_file.txt");
    destroy_mkfs();
    return 0;
};