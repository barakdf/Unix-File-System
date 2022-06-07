//
// Created by barak on 06/06/2022.
//

#include "ufs.h"

myopenfile open_files[MAX_FILES];
int bool_open_files[MAX_FILES];
int max_open_indx;
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

    /* allocate memory for d_Inodes */
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
    printf("num of d_Inodes: %d\n", sb.num_inodes);
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
//    printf("Max is : %d\n", max_open_indx);
    printf("SuperBlock info\n");
    printf("\t num d_Inodes %d\n", (sb.num_inodes));
    printf("\t num blocks %d\n", (sb.num_blocks));
    printf("\t size blocks %d\n", (sb.size_blocks));

    printf("d_Inodes\n");
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

int find_inode(char name[8]) {
    for (int i = 0; i < sb.num_inodes; ++i) {
        if(strcmp(inodes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int add_open_file(int fd) {
    for (int i = 0; i < MAX_FILES; ++i) {
        if(bool_open_files[i] == 0) {
            open_files[i].fd = fd;
            open_files[i].p_block = &dbs[inodes[fd].first_block];
            open_files[i].num_block = 1;
            bool_open_files[i] = 1;
            if (i > max_open_indx) {
                max_open_indx = i;
            }
            return 0;
        }
    }
    return -1;
}

struct myopenfile * find_opened_file(int fd) {
    for (int i = 0; i < max_open_indx; ++i) {
        if(bool_open_files[i]) {
            return &open_files[i];
        }
    }
    return NULL;
}

/** open a file */
int myopen(const char *pathname, int flags) {

    char path[80] = "";
    strcpy(path,pathname);
    int curr_fd;
    char *token;
    token = strtok(path, "/");
    curr_fd = find_inode(token);
    while (token != NULL) {
        if (curr_fd == -1) {

        }
        if (inodes[curr_fd].type == Directory) {
            token = strtok(NULL, "/");
            if (token == NULL) {
                printf("Path given is not a file!\n");
                exit(EXIT_FAILURE);
            }
            mydirent * curr_dir = myreaddir(curr_fd);
            int found = 0;
            for (int i = 0; i < curr_dir->num_of_files ; ++i) {
                if (curr_dir->d_Inodes[i].name == token) {
                    curr_fd = find_inode(token);
                    found = 1;
                }
            }
            if (!found) {
                printf("Did not find the inode in the directory\n");
                exit(EXIT_FAILURE);
            }
        } else {
            int add = add_open_file(curr_fd);
            if (add == -1) {
                printf("Not enough place for open files!\n");
            }
            return curr_fd;
        }
    }
    return -1;
}

/** close a file */
/** Iterate over the open_files bitmap, when we find the requested fd
     * we change his status to free, and the openfile struct -> fd to be -1, and return 0 for success.
     * If not found, return -1*/
int myclose(int myfd) {

    for (int i = 0; i < max_open_indx; ++i) {
        if(bool_open_files[i] == 1 && (open_files[i].fd == myfd)) {
            bool_open_files[i] = 0;
            open_files[i].fd = -1;
            return 0;
        }
    }
    return -1;
}
/* single append char to str func,
 * source : https://stackoverflow.com/questions/10279718/append-char-to-string-in-c*/
void strcat_c (char *str, char c)
{
    for (;*str;str++);
    *str++ = c;
    *str++ = 0;
}

void next_block(struct myopenfile* op) {
    op->p_block = &dbs[op->p_block->next_block_num];
    op->num_block++;
}

/** read from a file */
ssize_t myread(int myfd, void *buf, size_t count) {

    myopenfile *curr_file = find_opened_file(myfd);
    if(curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        exit(EXIT_FAILURE);
    }
    int space_left = BLOCK_SIZE - curr_file->pos;
    int num_of_blocks;
    if(count < space_left) {
        num_of_blocks = 1;
    } else {
        num_of_blocks = (((int )count - space_left)/BLOCK_SIZE) + 1;
    }
//    int curr_block = inodes[myfd].first_block;
    for (int i = 0; i < num_of_blocks; ++i) {
        while (curr_file->pos < BLOCK_SIZE) {
            strcat_c(buf, curr_file->p_block->data[curr_file->pos]);
            curr_file->pos++;
        }
        curr_file->pos = 0;
        next_block(curr_file);
    }
    return strlen(buf);
}

int extend_block(struct disk_block * block) {
    for (int i = 0; i < sb.num_blocks; ++i) {
        if(dbs[i].next_block_num == -1) {
            block->next_block_num = i;
            dbs[i].next_block_num = -2;
            return 0;
        }
    }
    return -1;
}


/** write to a file */
ssize_t mywrite(int myfd, const void *buf, size_t count) {
    int buf_pointer = 0;
    char *buffer = (char *) buf;
    myopenfile *curr_file = find_opened_file(myfd);
    if(curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        return buf_pointer;
    }
    while (buf_pointer < strlen(buffer)) {
        while (curr_file->pos < BLOCK_SIZE) {
            curr_file->p_block->data[curr_file->pos++] = buffer[buf_pointer++];
        }
        curr_file->pos = 0;

        /* in case we need to extend the file length */
        if(curr_file->p_block->next_block_num == -2) {
            int find = extend_block(curr_file->p_block);
            if (find == -1) {
                printf("Not enough space\n");
                return buf_pointer;
            }
        }

        next_block(curr_file);
    }
    return buf_pointer;
}

off_t mylseek(int myfd, off_t offset, int whence) {
    struct myopenfile * curr_file = find_opened_file(myfd);
    if(curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        exit(EXIT_FAILURE);
    }
    off_t off = offset;

    /* if the offset is from the current spot we add the offset to the pos
     * if the offset is from the start spot we assign the offset to the pos */
    if(whence == SEEK_CUR) {
        curr_file->pos += off;
        if(curr_file->pos < 0) {
            curr_file->pos = 0;
        }
    }
    else if(whence == SEEK_SET) {
        if(off < 0) {
            curr_file->pos = 0;
        } else {
            curr_file->pos = off;
        }
    }

    curr_file->num_block = (curr_file->pos / BLOCK_SIZE) + 1;
    curr_file->pos = (curr_file->pos % BLOCK_SIZE);
    curr_file->p_block = &dbs[inodes[myfd].first_block];

    for (int i = 0; i < curr_file->num_block - 1; ++i) {
        next_block(curr_file);
    }

    return curr_file->pos;
}

struct mydirent *myreaddir(int fd) {

}

