//
// Created by 97252 on 6/13/2022.
//

#include "myfile.h"
#include <stdarg.h>

int extended_block(struct disk_block *block) {
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
            int find = extended_block(p_block);
            if (find == -1) {
                printf("Not enough space\n");
                return buf_pointer;
            }
        }

        p_block = (struct disk_block *) &dbs[p_block->next_block_num];
    }
    return buf_pointer;
}

void fstrcat_c(char *str, char c) {
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
            fstrcat_c(buf, p_block->data[pos]);
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
        if (strcmp(mode, "r+") == 0) {
            my_File->mode = R_P;
        }
    }
    return my_File;
}

int myfclose(myFILE *stream) {
    /** if the mode is applying writing, then we need to write the changes to the source file,
     * if the writing failed -> return -1 for failure */
    if (stream->mode == A || stream->mode == W) {
        if (write_data(stream->fd, stream->data, stream->size) == -1) {
            return -1;
        }
    }
    /** free all the allocated data, and return 1 for success */
    free(stream->data);
    free(stream);
    return 1;

}


size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream) {

    /** size of total read length */
    int len = (int) (size * nmemb) + 1;
    char *buf = malloc(len);
    int pos = stream->seek_pos;
    for (int i = 0; i < len; ++i) {
        if (stream->seek_pos + i > stream->size) {
            break;
        }
        buf[i] = stream->data[stream->seek_pos + i];
        pos++;
    }
    stream->seek_pos = pos;
    buf[len] = 0;
    strncpy(ptr, buf, len);
    free(buf);

    return stream->seek_pos;

}

size_t myfwrite(const void *ptr, size_t size, size_t nmemb, myFILE *stream) {
    if (stream->mode == R) {
        printf("Wrong Flag, could not write data to file\n");
        return -1;
    }
    int len = size * nmemb;
    char *buf = (char *) ptr;

    /** if len of writing buf is exceeding the current stream data buffer,
     * increase the length of stream data, using a temporary buf*/
    if (stream->seek_pos + len > stream->size) {
        //allocate mem for temp and copy data from the stream
        char *temp = malloc(stream->size + 1);
        for (size_t i = 0; i < stream->size; ++i) {
            temp[i] = stream->data[i];
        }
        //free stream data
        free(stream->data);
        //allocate larger mem size for data
        stream->data = malloc(stream->size + len);
        //copy back the data content
        for (size_t i = 0; i < stream->size; ++i) {
            stream->data[i] = temp[i];
        }
        // we don't need temporary buf anymore, free.
        free(temp);
    }
    int pos = stream->seek_pos;
    /** write the buf to the data from curr pointer*/
    for (size_t i = 0; i < len; ++i) {
        stream->data[stream->seek_pos + i] = buf[i];
        pos++;
    }
    stream->seek_pos = pos;
    return stream->seek_pos;
}

int myfseek(myFILE *stream, long offset, int whence) {
    if (whence == SEEK_CUR) {
        stream->seek_pos += offset;
    } else if (whence == SEEK_END) {
        stream->seek_pos = stream->size + offset;
    } else if (whence == SEEK_SET) {
        stream->seek_pos = offset;
    }
    // if the offset moved the seek pointer below zero.
    if (stream->seek_pos < 0) {
        stream->seek_pos = 0;
    }
    return stream->seek_pos;
}

int myfscanf(myFILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *storage;
    int i = 0, j = 0;
    while (format != 0 && format[i] != 0) {
        if(format[i] == '%') {
            if(format[i+1] == 'd') {
                *(int *) va_arg(args, int *) = strtol(&stream->data[j], &storage, 10);
                j+= strlen(storage) - stream->size;
            }
            else if (format[i+1] == 'f') {
                *(double *) va_arg(args, double *) = strtol(&stream->data[j], &storage, 10);
                j+= strlen(storage) - stream->size;
            }
            else if (format[i + 1] == 'c') {
                *(char *) va_arg(args, char *) = stream->data[j];
            }
            i++;
        }
        i++;
        j++;
    }
    return 1;
}

int myfprintf(myFILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int len = strlen(format);
    int counter = 0;
    for (int i = 0; i < len; ++i) {
        /** if the next char is an argument symbol, we check what is the type and then write*/
        if (format[i] == '%') {
            if (format[i + 1] == 'd') { // represent Integer argument
                int temp_int = va_arg(args, int);
                myfwrite(&temp_int, sizeof(int ), 1, stream);
                counter++;
            } else if (format[i + 1] == 'f') { // represent float argument
                float temp_float = va_arg(args, double);
                myfwrite(&temp_float, sizeof(float ), 1, stream);
                counter++;
            } else if (format[i + 1] == 'c') { // represent char argument
                char temp_char = va_arg(args, int);
                myfwrite(&temp_char, sizeof (char ), 1, stream);
                counter++;
            }
            i++;
        } else {
            /** if the next char is not an argument symbol, we write the text until we get one.*/
            while (format[i] != '%' && i < len) {
                char temp_regular_char = format[i++];
                myfwrite(&temp_regular_char, sizeof (char ), 1, stream);
            }
            i--;
        }
    }
    return counter;
}