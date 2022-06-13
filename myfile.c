//
// Created by 97252 on 6/13/2022.
//

#include "myfile.h"

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

ssize_t write_data(int fd, const void *buf, size_t count) {
    int pos = 0;
    int buf_pointer = 0;
    char *buffer = (char *) buf;

    disk_block *p_block = (struct disk_block *) &dbs[inodes[fd].first_block];

    while (buf_pointer < strlen(buffer)) {
        while (pos < BLOCK_SIZE) {
            p_block->data[pos++] = buffer[buf_pointer++];
        }
        pos = 0;

        /* in case we need to extend the file length */
        if (p_block->next_block_num == -2) {
            int find = extend_block(p_block);
            if (find == -1) {
                printf("Not enough space\n");
                return buf_pointer;
            }
        }

        p_block = (struct disk_block *) &dbs[p_block->next_block_num];
    }
    return buf_pointer;
}

void strcat_c(char *str, char c) {
    for (; *str; str++);
    *str++ = c;
    *str++ = 0;
}

size_t read_data(int fd, char *buf, int count) {
    int pos = 0;
    int space_left = BLOCK_SIZE;
    int num_of_blocks;
    disk_block *p_block = (struct disk_block *) &dbs[inodes[fd].first_block];
    if (count < space_left) {
        num_of_blocks = 1;
    } else {
        num_of_blocks = (((int) count) / BLOCK_SIZE) + 1;
    }
//    int curr_block = inodes[myfd].first_block;
    for (;;) {
        while (pos < BLOCK_SIZE) {
            strcat_c(buf, p_block->data[pos]);
            pos++;
        }
        pos = 0;
        if (p_block->next_block_num < 0) {
            break;
        }
        p_block = (struct disk_block *) &dbs[p_block->next_block_num];
    }
    return strlen(buf);
}

myFILE *myfopen(const char *pathname, const char *mode) {
    int fd = myopen(pathname, 0);
    myFILE *my_File = (struct myFILE *) malloc(sizeof(myFILE));

    my_File->fd = fd;
    my_File->size = inodes[fd].size;
    /** In case the requested type file is Directory */
    if (inodes->type == Directory) {
//        mydirent *f_data = (struct mydirent *)&dbs[inodes[fd].first_block];
//        my_File->type = Directory;
//        my_File->dir = malloc(sizeof (struct mydirent));
//        for (int i = 0; i < f_data->num_of_files; ++i) {
//            my_File->dir->d_Inodes[i] = f_data->d_Inodes[i];
//        }
//        my_File->dir->num_of_files = f_data->num_of_files;
//        strcpy(my_File->dir->d_name,f_data->d_name);
        return NULL;
    } else {
        disk_block *f_data = (struct disk_block *) &dbs[inodes[fd].first_block];
        my_File->data = malloc(inodes[fd].size);
        if (strcmp(mode, "w") == 0) {
            my_File->mode = W;
            memset(my_File->data, 0, inodes[fd].size);

        } else {
            my_File->mode = A;
            size_t bytes_read = read_data(fd, my_File->data, inodes[fd].size);
            if (bytes_read == -1) {
                printf("Could not recieve data \n");
            }

        }
        /** if the mode is "append" type (key - a),
         * the seek pointer will be set to the end of the data,
         * else will set to 0*/
        if (strcmp(mode, "a") == 0) {
            my_File->seek_pos = my_File->size;
            my_File->mode = A;
        } else {
            my_File->seek_pos = 0;
        }
        if(strcmp(mode, "r+") == 0) {
            my_File->mode = R_P;
        }
    }
    return my_File;
}

int myfclose(myFILE *stream) {
    /** if the mode is applying writing, then we need to write the changes to the source file,
     * if the writing failed -> return -1 for failure */
    if(stream->mode == A || stream->mode == W) {
        if (write_data(stream->fd, stream->data, stream->size) == -1) {
            return -1;
        }
    }
    /** free all the allocated data, and return 1 for success */
    free(stream->data);
    free(stream);
    return 1;

}
