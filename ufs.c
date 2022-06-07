//
// Created by barak on 06/06/2022.
//

#include "ufs.h"

myopenfile open_files[MAX_FILES];
int mkfs_bool = 0;


/** Initialize new File System
 *  receive parameter s -> size of the file system.*/
void mymkfs(int s) {
    mkfs_bool = 1;
    // initializing super block struct.
    int inodes_capacity = (s / 10);
    int blocks_capacity = s - (inodes_capacity) - (sizeof(struct superBlock));
    sb.num_inodes = (inodes_capacity) / (sizeof(struct inode));
    sb.num_blocks = blocks_capacity / (sizeof(struct disk_block));
    sb.size_blocks = sizeof(struct disk_block);

    /* allocate memory for inodes */
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);

    for (int i = 0; i < sb.num_inodes; ++i) {
        inodes[i].size = -1;
        inodes[i].first_block = -1;
        inodes[i].type = File;
        strcpy(inodes[i].name, "welcome");
    }
    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);

    for (int i = 0; i < sb.num_blocks; ++i) {
        dbs[i].next_block_num = -1;

        strcpy(dbs[i].data, "");
    }

}



/** write the File System */
//void sync_fs() {

//}
void save(const char *target) {
    FILE *s_file;
    s_file = fopen(target, "w");
//        fwrite("hello", 1,5,s_file);
    fwrite(&sb, sizeof(struct superBlock), 1, s_file);

    /* write each inode by order */
    for (int i = 0; i < sb.num_inodes; ++i) {
        fwrite(&(inodes[i]), sizeof(struct inode), 1, s_file);
    }
    /* write each block by order */
    for (int i = 0; i < sb.num_blocks; ++i) {
        fwrite(&(dbs[i]), sizeof(struct disk_block), 1, s_file);
    }

    fclose(s_file);
}

void load(const char *source) {
    FILE *t_file;
    t_file = fopen(source, "r");
    fread(&sb, sizeof(struct superBlock), 1, t_file);

    /* read each inode by order */
    printf("num of inodes: %d\n", sb.num_inodes);
    for (int i = 0; i < sb.num_inodes; ++i) {
        fread(&(inodes[i]), sizeof(struct inode), 1, t_file);
    }
    /* read each block by order */
    for (int i = 0; i < sb.num_blocks; ++i) {
        fread(&(dbs[i]), sizeof(struct disk_block), 1, t_file);
    }
    fclose(t_file);
}

/** Load a File System */
int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long mountflags, const void *data) {
    if (source != NULL) {
        load(source);
    }

    if (target != NULL) {
        save(target);
    }


    return 0;
}

void print_fs() {
    printf("SuperBlock info\n");
    printf("\t num inodes %d\n", (sb.num_inodes));
    printf("\t num blocks %d\n", (sb.num_blocks));
    printf("\t size blocks %d\n", (sb.size_blocks));

    printf("inodes\n");
    for (int i = 0; i < sb.num_inodes; ++i) {
        printf("\tblock size: %d || block : %d || name: %s\n", inodes[i].size, inodes[i].first_block, inodes[i].name);
    }

    printf("blocks\n");
    for (int i = 0; i < sb.num_blocks; ++i) {
        printf("\tblock num: %d || next block: %d\n", i, dbs[i].next_block_num);
    }
}

int allocate_file(char name[8]) {
    int available_inode = find_empty_inode();
    if (available_inode == -1) {
        exit(EXIT_FAILURE);
    }

    int available_block = find_empty_block();
    if (available_block == -1) {
        exit(EXIT_FAILURE);
    }

    inodes[available_inode].first_block = available_block;
    dbs[available_block].next_block_num = -2;

    strcpy((inodes[available_inode].name), name);

}

int find_empty_inode() {
    for (int i = 0; i < sb.num_inodes; ++i) {
        if(inodes[i].first_block == -1) {
            return i;
        }
    }
    printf("There is not a free inode!\n");
    return -1;
}

int find_empty_block() {
    for (int i = 0; i < sb.num_blocks; ++i) {
        if(dbs[i].next_block_num == -1) {
            return i;
        }
    }
    printf("There is not a free block!\n");
    return -1;
}

void set_filesize(int file_num, int size) {
    int temp = size + BLOCK_SIZE - 1;

    int total_blocks = temp / BLOCK_SIZE;

    int block_num = inodes[file_num].first_block;
    total_blocks --;

    /* in case we need to increase the file blocks */
    while (total_blocks > 0) {
        int next_block_n = dbs[block_num].next_block_num;
        if( next_block_n == -2) {
            int available_b = find_empty_block();
            dbs[block_num].next_block_num = available_b;
            dbs[available_b].next_block_num = -2;
        }
        // forwarding the current block_number;
        block_num = dbs[block_num].next_block_num;
        total_blocks--;
    }
    dbs[block_num].next_block_num = -1;

}

void write_byte (int file_num, int pos, char *data) {

}

