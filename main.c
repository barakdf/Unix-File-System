//#include <stdio.h>
//#include "stdlib.h"
//#include "ufs.h"
//#include "fcntl.h"
//
//int main() {
//    mymkfs(10000);
//
//
////    mymount(NULL, "my_file.txt", NULL, NULL, NULL);
////    allocate_file("hello");
//    int ok = myopen("root/hello/OKOKOK", O_CREAT);
//    int sababa = myopen("root/hello/SABABA", O_CREAT);
//    char write_text[1024];
//    strcpy(write_text, "Hello World");
//    printf("size of write text is -> %d\n", strlen(write_text));
//    int pointer = mywrite(ok, write_text, strlen(write_text));
//    printf("curr pointer -> %d\n", pointer);
//    mylseek(ok,0,SEEK_SET);
//    char read_text[1024];
//    myread(ok, read_text, 5);
//    printf("BACK -> %s\n", read_text);
//    myclose(ok);
//    myclose(sababa);
//
//    print_fs();
//    save("my_file.txt");
//////    mymount("my_file.txt", NULL, NULL, NULL, NULL);
////    myopen("hi/hello", NULL);
//    destroy_mkfs();
//    return 0;
//}
