#include <stdio.h>
#include "stdlib.h"
#include "ufs.h"
#include "fcntl.h"

int main() {
    mymkfs(10000);


//    mymount(NULL, "my_file.txt", NULL, NULL, NULL);
//    allocate_file("hello");
    myopen("root/hello/OKOKOK", O_CREAT);
    myopen("root/hello/SABABA", O_CREAT);

    print_fs();
    save("my_file.txt");
////    mymount("my_file.txt", NULL, NULL, NULL, NULL);
//    myopen("hi/hello", NULL);
    return 0;
}
