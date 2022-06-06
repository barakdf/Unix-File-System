//
// Created by barak on 06/06/2022.
//

#ifndef UNIX_FILE_SYSTEM_UFS_H
#define UNIX_FILE_SYSTEM_UFS_H

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define NAME_SIZE 8
#define BLOCK_SIZE 512
#define MAX_FILES 10000

enum inode_type{File, Directory};


/** SuperBlock Struct :
 *      meta info about the file system
 *      number of inodes
 *      number of disk blocks
 *      size of the disk blocks*/
typedef struct superBlock {
    int num_inodes;
    int num_blocks;
    int size_blocks;

}superBlock;

typedef struct inode {
    int size;
    char name[NAME_SIZE];
    int type;
} inode;

typedef struct disk_block{
    int next_block_num;
    char data[BLOCK_SIZE];
} disk_block;

typedef struct mydirent {
    char d_name[NAME_SIZE];
    
}mydirent;
superBlock sb;
inode *inodes;
disk_block *dbs;

/** Initialize new File System */
void mymkfs(int );

/** Load a File System */
void mount_fs();

/** write the File System */
void sync_fs();


int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long mountflags, const void *data);

#endif //UNIX_FILE_SYSTEM_UFS_H
