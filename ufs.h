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
enum block_status{Used = -2, Free = -1};

typedef struct myopenfile {
    int fd;
    int pos;
} myopenfile;


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
    int first_block;
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
int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long mountflags, const void *data);

/** open a file */
int myopen(const char *pathname, int flags);
/** close a file */
int myclose(int myfd);

/** read from a file */
ssize_t myread(int myfd, void *buf, size_t count);
/** write to a file */
ssize_t mywrite(int myfd, const void *buf, size_t count);

/** move the adjusting pointer in a specific file */
off_t mylseek(int myfd, off_t offset, int whence);

//
//
//
void print_fs();

/** read a File System */
void load(const char *);

/** write the File System */
void save(const char* target);

/* @ allocate_file:
 *      - find an empty inode
 *      - claim it
 *      - find\claim a disk block
 *      - return the file descriptor
 *
 * In function CALLS -> (@find_empty_node, @find_empty_block)
 * */
int allocate_file(char name[8]);
int find_empty_inode();
int find_empty_block();

void set_filesize(int file_num, int size);
void write_byte (int file_num, int pos, char *data);

#endif //UNIX_FILE_SYSTEM_UFS_H
