# A3: Broken Song

Learning Outcomes
-----------------

1.  **Write** a Makefile to compile a simple C program.
2.  **Use** file stream functions to read and write binary files.
3.  **Use** the thread library to create and join threads.
4.  **Create** a program that will concatenate binary files.
5.  **Create** a program that can utilize a mutex to share a data structure between threads.
6.  **Use** memory allocator functions.

Specifications
--------------

Your program, which will be invoked as `defrag` will have to traverse through a directory tree looking for pieces of an mp3, each piece's filename will be made up of an non-negative integer from 0 to the total number of files and the file-extension `.bin` The file contents of those pieces should be concatenated into an output file in the order the numbers imply.

Your program will be given two arguments, a path to the directory tree to explore, and the name of the output file. **The starting directory in the traversal will always only contain subdirectories.** Your program must explore those subdirectories using asynchronous threads. Remember to compile and link your program with the `-lpthread` flag.

**File numbers will always start at 0**, each integer is only used once **with no missing numbers**, and the order the files are discovered is incredibly unlikely to be in numeric order.

### Threads

You must start up a thread **for each top level directory found in the given path**. It is not recommended to start up more threads than this (such as for every directory encountered). Each thread can recursively traverse the directory structure when discovering another folder or utilize the mutex to append data to the shared data structure. **Do not follow symbolic links**, they are placed into the directory tree as a trap and should be ignored.

### Mutex

A mutex is just a "key" that is shared between different threads in order to "lock" a specific section of code referred to as a "critical section". This means **only one thread** is allowed into the critical section at any given time. A thread must completely finish executing code inside of the critical section before it is able to release the lock and let other threads obtain the "key".

We will use an approach where threads sit and wait for a key (so called "blocking"). This means **you must do as much as possible outside of the critical section**. This can be opening and reading in the entire content of the file. The actual critical section really only needs to be ~5 lines of code. If your critical section is larger than this, or if it is so large that it encompases the entire thread function, you are in danger of making your program completely single-threaded **which you will lose points for doing**.

The **only** time you need to put code into a critical section is when you are reading/writing shared resources!

### Restrictions

*   You must compile your program using the flags `std=gnu11`, `-Werror` and `-Wall`.
*   You must use a global, shared array to hold file fragment data.
*   Your program **must actually be multithreaded**, and have the **smallest possible critical section**. You will lose implementation points for this (or worse) during code review!

### Submission

Submit only your completed `defrag.c` file and associated `Makefile`. When `make` is executed in the directory, a binary named `defrag` should be created. If not, the autograder will fail. You can make a submission as many times as you'd like in order to see the autograder feedback. The maximum points you can receive from the autograder is 14 and the remaining 12 points are from code review.

### Functions

Here is a list of functions that are used by the solution programs. This might help in completing the assignment:

*   `abort()`
*   `atoi()`
*   `chdir()`
*   `closedir()`
*   `exit()`
*   `fclose()`
*   `fopen()`
*   `fread()`
*   `free()`
*   `fwrite()`
*   `malloc()`
*   `opendir()`
*   `printf()`
*   `pthread_create()`
*   `pthread_join()`
*   `pthread_mutex_lock()`
*   `pthread_mutex_unlock()`
*   `puts()`
*   `readdir()`
*   `realloc()`
*   `stat()`
*   `stderr()`
*   `strcat()`
*   `strcmp()`
*   `strdup()`
*   `strlen()`

### Example

One such directory tree is given in the starting code. You would run your program like:

    $ ./defrag dirs music.mp3

The resulting file [should have an MD5 hash](https://en.wikipedia.org/wiki/MD5) as shown:

    $ md5sum music.mp3 
    96726c8464765869318032405caed000  music.mp3

Extra Credit
------------

Once you've obtained the complete `.mp3` file, there is a hidden CTF challenge that will involve **forensics**, **cryptography** and **web** disciplines. If you can obtain the flag and send it to me, you can receive some extra credit and/or a GitHub sticker.

