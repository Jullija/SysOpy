#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>
#include <libgen.h>

long long size = 0;

int HokusPokus(const char *fpath, const struct stat *sb, int typeflag){
    if (!S_ISDIR(sb->st_mode)){ //if it's not a directory
        char * toGetFilename = malloc(strlen(fpath) * sizeof(char) * 2); //*2 to make it work
        strcpy(toGetFilename, fpath); //copy fpath to toGetFilename variable
        printf("size: %ld, name: %s\n", sb->st_size, basename(toGetFilename));
        size += sb->st_size;
        free(toGetFilename);
    } else {
        printf("This is directory: %s\n", fpath);
    }
    return 0;
}

int main(int argc, char ** argv){

    if (argc != 1){
        printf("Wrong number of arguments");
        exit(-1);
    }


    if (ftw(argv[1], HokusPokus, 10) == -1){
        perror("ftw");
        exit(EXIT_FAILURE);
    }

    printf("size of files in directory: %lld\n", size);


}