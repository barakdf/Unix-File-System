//
// Created by barak on 06/06/2022.
//

#include "ufs.h"
#include "fcntl.h"

myopenfile open_files[MAX_FILES];
int bool_open_files[MAX_FILES];
int max_open_indx;
int mkfs_bool = 0;
int OFF_SET_BLOCK;


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

//    bitmap_blocks = malloc(sizeof(int) * sb.num_blocks);

    /* allocate memory for d_Inodes */
    inodes = malloc(sizeof(struct inode) * sb.num_inodes);

    for (int i = 0; i < sb.num_inodes; ++i) {
        inodes[i].size = -1;
        inodes[i].first_block = -1;
        inodes[i].type = File;
        strcpy(inodes[i].name, "");
    }
    OFF_SET_BLOCK = sizeof(struct disk_block);
    dbs = malloc(sizeof(struct disk_block) * sb.num_blocks);
    memset(dbs, 0, sizeof(struct disk_block) * sb.num_blocks);

    for (int i = 0; i < sb.num_blocks; ++i) {
        disk_block *disk = (struct disk_block *) &dbs[i];
        disk->next_block_num = -1;
        strcpy(disk->data, "");
    }


//    inodes[0].type = Directory;
//    strcpy(inodes[0].name, "root");
//    inodes[0].first_block = 0;

    memset(&dbs[0], 0, sizeof(struct disk_block));
//    mydirent *root_dir = (struct mydirent *) &dbs[0];
//    strcpy(root_dir->d_name, "root");
//    root_dir->num_of_files = 0;
//    bitmap_blocks[0] = 1;
    myopendir("root");

    printf("Done here mkmfs\n");

}



/** write the File System */
//void sync_fs() {

//}
void save(const char *target) {
    FILE *s_file;
    printf("Saving file\n");
    s_file = fopen(target, "w");
//        fwrite("hello", 1,5,s_file);
    fwrite(&sb, sizeof(struct superBlock), 1, s_file);

    /* write each inode by order */
        fwrite(inodes, sizeof(struct inode), sb.num_inodes, s_file);

    /* write each block by order */
//        disk_block *disk_temp = (struct disk_block *) &dbs[i * OFF_SET_BLOCK];
//        printf("%s\n", &dbs[i * OFF_SET_BLOCK]);
    fwrite(dbs, sizeof (struct disk_block), sb.num_blocks, s_file);
    fseek(s_file, 0, SEEK_SET);

    fclose(s_file);
}

void load(const char *source) {
    FILE *t_file;
    t_file = fopen(source, "r");
//    bitmap_blocks = malloc(sb.num_blocks);

    fread(&sb, sizeof(struct superBlock), 1, t_file);

    /* allocate memory from inodes and blocks, receive data from super block  */
    inodes = malloc(sizeof(struct inode) * (sb.num_inodes+1));
    dbs = malloc(sizeof (struct disk_block)* (sb.num_blocks));
    memset(dbs,0,sizeof (struct disk_block)* (sb.num_blocks));

    printf("num of d_Inodes: %d\n", sb.num_inodes);




    /* read each block by order */
//    OFF_SET_BLOCK = (sizeof(struct disk_block));
//    disk_block *disk_temp = (struct disk_block *) &dbs[i];

    fread(inodes, sizeof(struct inode) , sb.num_inodes, t_file);
    printf("check %d\n",sb.num_blocks);
    fread(dbs, (sizeof (struct disk_block))*(sb.num_blocks - 1),1 , t_file);
    printf("Hellooooo\n");
    fseek(t_file, 0, SEEK_SET);
    fclose(t_file);

    printf("num of blocks %d\n", sb.num_blocks);
    for (int i = 0; i < sb.num_blocks; ++i) {
        if (dbs[i].next_block_num == -1) {
            bitmap_blocks[i] = 0;
        } else {
            bitmap_blocks[i] = 1;
        }

    }
    OFF_SET_BLOCK = sizeof (struct disk_block);
    printf("Hellooooo\n");


}

int destroy_mkfs() {
    free(inodes);
    free(dbs);
//    free(bitmap_blocks);
}

/** Load a File System */
int mymount(const char *source, const char *target,
            const char *filesystemtype, unsigned long mountflags, const void *data) {

    if (target == NULL && source == NULL) {
        return -1;
    }

    if (target != NULL) {
        load(target);
    }

    if (source != NULL) {
        save(source);
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

    mydirent *test = (mydirent *) &dbs[inodes[0].first_block];

    printf("blocks\n");
    for (int i = 0; i < sb.num_blocks; ++i) {
        printf("\tblock num: %d || block data:  %s   -, %d\n", i, test->d_name, OFF_SET_BLOCK);
    }
    for (int i = 0; i < sb.num_blocks; ++i) {

        printf("%d,", bitmap_blocks[i]);
    }
}

int allocate_file(char name[8]) {
    int available_inode = find_empty_inode();
    if (available_inode == -1) {
        printf("No space for Inodes\n");
        return -1;
    }

    int available_block = find_empty_block();
    if (available_block == -1) {
        printf("No space for blocks\n");
        return -1;
    }

    inodes[available_inode].first_block = available_block;
    disk_block *temp = (struct disk_block *) &dbs[available_block];
    temp->next_block_num = -2;

    inodes[available_inode].type = File;
    strcpy((inodes[available_inode].name), name);
    return available_inode;
}

int find_empty_inode() {
    for (int i = 0; i < sb.num_inodes; ++i) {
        if (inodes[i].first_block == -1) {
            return i;
        }
    }
    printf("There is not a free inode!\n");
    return -1;
}

int find_empty_block() {
    for (int i = 0; i < sb.num_blocks; ++i) {
        if (bitmap_blocks[i] == 0) {
            bitmap_blocks[i] = 1;
            return i;
        }
    }
    printf("There is not a free block!\n");
    return -1;
}

//void set_filesize(int file_num, int size) {
//    int temp = size + BLOCK_SIZE - 1;
//
//    int total_blocks = temp / BLOCK_SIZE;
//
//    int block_num = inodes[file_num].first_block;
//    total_blocks--;
//
//    /* in case we need to increase the file blocks */
//    while (total_blocks > 0) {
//        int next_block_n = dbs[block_num].next_block_num;
//        if (next_block_n == -2) {
//            int available_b = find_empty_block();
//            dbs[block_num].next_block_num = available_b;
//            dbs[available_b].next_block_num = -2;
//        }
//        // forwarding the current block_number;
//        block_num = dbs[block_num].next_block_num;
//        total_blocks--;
//    }
//    dbs[block_num].next_block_num = -1;
//
//}

void write_byte(int file_num, int pos, char *data) {

}

/** Return the FD, in case not found, Return -1*/
int find_inode(char name[8]) {
    for (int i = 0; i < sb.num_inodes; ++i) {
        if (strcmp(inodes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int add_open_file(int fd) {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (bool_open_files[i] == 0) {
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

struct myopenfile *find_opened_file(int fd) {
    for (int i = 0; i < max_open_indx; ++i) {
        if (bool_open_files[i] && (open_files[i].fd == fd)) {
            return &open_files[i];
        }
    }
    return NULL;
}

/** open a file */
int myopen(const char *pathname, int flags) {

    /* count how many path arguments */
    char path[80] = "";
    strcpy(path, pathname);
    char *token;
    int dir_counter = 0;
//    printf("Path: %s \n", path);
    token = strtok(path, "/");

//    printf("Path: %s \n", path);
//    printf("TOKEN: %s \n", token);

    while (token != NULL) {
        dir_counter += 1;
        token = strtok(NULL, "/");
//        printf("TOKEN: %s \n", token);
    }

//    printf("Path: %s \n", path);
    strcpy(path, pathname);
    printf("count: %d\n Path: %s \n", dir_counter, path);

    /* allocate space for all the arguments */
    char **temp_path = (char **) malloc(dir_counter * sizeof(char *));
    for (int i = 0; i < dir_counter; i++) {
        temp_path[i] = (char *) calloc(20, sizeof(char));
    }

    /* copy all the path arguments to the 2D (char *) array */
    token = strtok(path, "/");
    int i = 0;
    while (token != NULL) {
        strcpy(temp_path[i++], token);
        token = strtok(NULL, "/");
    }
    /* current file descriptor */
    int curr_fd = -1;
    /* current inode -> always the first directory is "root" */
    inode *curr_dir_inode = &inodes[0];

    printf("CHECK \n");

    // temp_path = { [], [], [],...}
    /** Handle only with the directories */
    for (int p = 1; p < dir_counter - 1; ++p) {
        mydirent *curr_directory = (struct mydirent *) &dbs[curr_dir_inode->first_block];
        printf("name of dir %s\n", curr_directory->d_name);
        printf("HERE %d\n", curr_directory->num_of_files);
//        printf("HERE %s\n", curr_directory->d_Inodes[0]->name);


        /** Iterate over the directory files, and check if the current path argument is present */
        for (int j = 0; j < curr_directory->num_of_files; ++j) {
            // if we find the file
            printf("name of dir child -> %s\n", inodes[curr_directory->d_fds[j]].name);
            if (strcmp(inodes[curr_directory->d_fds[j]].name, temp_path[p]) == 0) {
                curr_fd = find_inode(temp_path[p]);

                break;
            } else { // if we dont find the file
                curr_fd = -1;
            }
        }
        /** in case the name does not exist in the inodes. */
        if (curr_fd == -1) {

            if (flags != O_CREAT) {
                printf("file does not exist in the system!\n");
                return -1;
            }


            /** if O_CREAT is active, open new directory with the path-argument name*/

            int dir_fd = myopendir(temp_path[p]);
            printf("name of dir %s\n", curr_directory->d_name);

            printf("DIR_FD IS: %d\n", dir_fd);
            if (dir_fd == -1) {
                return -1;
            }

            /** update new inode address to inode array of the current directory , assign the dir_fd */

            printf("%d\n", curr_directory->num_of_files);
            curr_directory->d_fds[curr_directory->num_of_files] = dir_fd;
            curr_directory->num_of_files++;

            curr_fd = dir_fd;

        }
        curr_dir_inode = &inodes[curr_fd];

    }

    /** Handle with file itself */
    printf("CHECK _FD: %d\n", curr_fd);
    mydirent *curr_directory = (struct mydirent *) &dbs[curr_dir_inode->first_block];
    printf("CHECK_num files %d\n", curr_directory->num_of_files);
    curr_fd = -1;
    for (int j = 0; j < curr_directory->num_of_files; ++j) {
        if (strcmp(inodes[curr_directory->d_fds[j]].name, temp_path[dir_counter - 1]) == 0) {
            curr_fd = find_inode(temp_path[dir_counter - 1]);
            break;
        } else {
            curr_fd = -1;
        }
    }

    int file_fd;
    if (curr_fd == -1) {


        if (flags != O_CREAT) {
            return -1;
        }
        file_fd = allocate_file(temp_path[dir_counter - 1]);
        if (file_fd == -1) {
            printf("Allocate_file failed\n");
            return -1;
        }
        curr_fd = file_fd;
        printf("file_fd: %d\n", curr_fd);
        curr_directory->d_fds[curr_directory->num_of_files] = curr_fd;
        curr_directory->num_of_files++;
    }

    printf("FILE FOUND\n");

    /** FREE the temp_path */
    for (i = 0; i < dir_counter; i++) {
        free(temp_path[i]);
    }
    free(temp_path);


    /** add the file descriptor to open files container */
    int add = add_open_file(curr_fd);
    if (add == -1) {
        printf("Not enough place for open files!\n");
    }
    return curr_fd;
}

/** close a file */
/** Iterate over the open_files bitmap, when we find the requested fd
     * we change his status to free, and the openfile struct -> fd to be -1, and return 0 for success.
     * If not found, return -1*/
int myclose(int myfd) {

    for (int i = 0; i < max_open_indx; ++i) {
        if (bool_open_files[i] == 1 && (open_files[i].fd == myfd)) {
            bool_open_files[i] = 0;
            open_files[i].fd = -1;
            return 0;
        }
    }
    return -1;
}

/* single append char to str func,
 * source : https://stackoverflow.com/questions/10279718/append-char-to-string-in-c*/
void strcat_c(char *str, char c) {
    for (; *str; str++);
    *str++ = c;
    *str++ = 0;
}

void next_block(struct myopenfile *op) {
    op->p_block = &dbs[op->p_block->next_block_num];
    op->num_block++;
}

/** read from a file */
ssize_t myread(int myfd, void *buf, size_t count) {
//    printf("%s\n", buf);
    printf("HERE READ \n");
    myopenfile *curr_file = find_opened_file(myfd);
    char *buffer = (char *) buf;
    memset(buffer,0,count);
    if (curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        exit(EXIT_FAILURE);
    }
    int space_left = BLOCK_SIZE - curr_file->pos;
    int num_of_blocks;
    if (count < space_left) {
        num_of_blocks = 1;
    } else {
        num_of_blocks = (((int) count - space_left) / BLOCK_SIZE) + 1;
    }
//    int curr_block = inodes[myfd].first_block;
    for (int i = 0; i < num_of_blocks; ++i) {
        while (curr_file->pos < BLOCK_SIZE && count > 0) {
            strcat_c(buffer, curr_file->p_block->data[curr_file->pos]);
            curr_file->pos++;
            count--;
        }
        curr_file->pos = 0;
        next_block(curr_file);
    }
    return strlen(buffer);
}

int extend_block(struct disk_block *block) {
    for (int i = 0; i < sb.num_blocks; ++i) {
        if (bitmap_blocks[i] == 0) {
            block->next_block_num = i;
            disk_block *temp = (struct disk_block *) &dbs[i];
            temp->next_block_num = -2;
            return 0;
        }
    }
    return -1;
}


/** write to a file */
ssize_t mywrite(int myfd, const void *buf, size_t count) {
    printf("%s\n", (char*)buf);
    int buf_pointer = 0;
    char *buffer = (char *) buf;
    myopenfile *curr_file = find_opened_file(myfd);
    if (curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        return buf_pointer;
    }
    while (buf_pointer < strlen(buffer) ) {
        while (curr_file->pos < BLOCK_SIZE && count > 0) {
            curr_file->p_block->data[curr_file->pos++] = buffer[buf_pointer++];
            count--;
        }
        curr_file->pos = 0;

        /* in case we need to extend the file length */
        if (curr_file->p_block->next_block_num == -2) {
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
    struct myopenfile *curr_file = find_opened_file(myfd);
    if (curr_file == NULL) {
        printf("File is not opened or does not exist\n");
        return -1;
    }
    off_t off = offset;

    /* if the offset is from the current spot we add the offset to the pos
     * if the offset is from the start spot we assign the offset to the pos */
    if (whence == SEEK_CUR) {
        curr_file->pos += off;
        if (curr_file->pos < 0) {
            curr_file->pos = 0;
        }
    } else if (whence == SEEK_SET) {
        if (off < 0) {
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

/** allocate inode and block for directory, return fd, if not success, return -1 */
int myopendir(const char *name) {
    int available_inode = find_empty_inode();
    if (available_inode == -1) {
        printf("No space for Inodes\n");
        return -1;
    }

    int available_block = find_empty_block();
    if (available_block == -1) {
        printf("No space for blocks\n");
        return -1;
    }

    printf("AVAILABLE INODE: %d, BLOCK:%d\n", available_inode, available_block);
    inodes[available_inode].first_block = available_block;
    mydirent *temp = (struct mydirent *) &dbs[available_block];
    temp->num_of_files = 0;
    strcpy(temp->d_name, name);


    inodes[available_inode].type = Directory;
    strcpy((inodes[available_inode].name), name);
    return available_inode;
}

struct mydirent *myreaddir(int fd) {

}

