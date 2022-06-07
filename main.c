#include <stdio.h>
#include "stdlib.h"
#include "ufs.h"

int main() {
    mymkfs(1000);


    mymount(NULL, "my_file.txt", NULL, NULL, NULL);
    allocate_file("HELLO");
    save("my_file.txt");
//    mymount("my_file.txt", NULL, NULL, NULL, NULL);
    print_fs();
    return 0;
}
