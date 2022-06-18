# Unix-File-System


This project represent a Unix file system in a file.

**Here is the instruction of how to use:**

- Before you start

<div dir='ltr'>

    make all


</div>


###Create new ufs

* mymkfs(s)
  * Parameter s will be the size of the file system file.


### Load\Save
* mymount()
  * First parameter is the path where you want to save the mounted file_system.
  * Second parameter is the path of the loaded file_system.




###Handle files in the system
In order to read/write into a file, you must open it first using  myopen()
* myopen(path, flag)
  * Path parameter defines which file to open from the f_system.
  * Flag parameter determine the decisions of the program in case the requested file or any directory in the path do not exist.
    * Pass O_CREAT flag to the function to open a new file in case it does not exist.
  * RETURN the file descriptor of the file.

Write to file:
* mywrite(fd,buf,count)
  * fd parameter will be the file descriptor of the file, returned from myopen()
  * buf parameter is a provided char* from the user that contain the context of what to write.
  * count parameter is the off-set that tells the program how many chars to write.
  * RETURN the number of writen bytes


read from file:

* myread(fd, buf, count)
  * fd parameter will be the file descriptor of the file, returned from myopen()
  * buf parameter is a provided char* from the user that will be filled with the content from the file.
  * count parameter is the off-set that tells the program how many chars to read from the file.
  * RETURN the number of bytes that we read.

handle write/read file pointer:

* mylseek(fd, offset, whence)
  * fd parameter will be the file descriptor of the file, returned from myopen()
  * offset will be the number of bytes that the pointer will move.
  * whence is a flag that set the starting point of the pointer before offset and support  3 options:
    * SEEK_SET - set the pointer to the beginning of the file.
    * SEEK_CUR - current position of the pointer
    * SEEK_END - set the pointer to the end of the file.
  * RETURN the position of the pointer.
### Test 
<div dir='ltr'>

    ./test


</div>

#### At the end don't forget to call destroy_mkmfs()

<div dir='ltr'>

    ./make clean


</div>