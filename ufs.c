//
// Created by barak on 06/06/2022.
//

#include "ufs.h"

/** Initialize new File System
 *  receive parameter s -> size of the file system.*/
void mymkfs(int s) {

    // initializing super block struct.
    int inodes_capacity = (s/10);
    int blocks_capacity = s - (inodes_capacity) - (sizeof (struct superBlock));
    sb.num_inodes = (inodes_capacity) / (sizeof (struct inode));
    sb.num_blocks = blocks_capacity / (sizeof (struct disk_block));
    sb.size_blocks = sizeof (struct disk_block);

    /* allocate memory for inodes */
    inodes = malloc(sizeof (struct inode) * sb.num_inodes);

    for (int i = 0; i < sb.num_inodes; ++i) {
        inodes[i].size = -1;
        inodes[i].type = File;
        strcpy(inodes[i].name, "");
    }
    dbs = malloc(sizeof (struct disk_block) * sb.num_blocks);

    for (int i = 0; i < sb.num_blocks; ++i) {
        dbs[i].next_block_num = -1;

        strcpy(inodes[i].name, "");
    }

}

/** Load a File System */
void mount_fs() {

}

/** write the File System */
//void sync_fs() {

//}

int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long mountflags, const void *data) {
    FILE *t_file;
    t_file = fopen(target, "r+");

    FILE *s_file;
    s_file = fopen(target, "w+");

    fwrite(&sb, sizeof (struct superBlock), 1, s_file);

    /* write each inode by order */
    for (int i = 0; i < sb.num_inodes; ++i) {
        fwrite(&(inodes[i]), sizeof (struct inode), 1, s_file);
    }
    for (int i = 0; i < sb.num_blocks; ++i) {
        fwrite(&(dbs[i]), sizeof (struct disk_block), 1, s_file);
    }

    fclose(s_file);

    return 0;
}

