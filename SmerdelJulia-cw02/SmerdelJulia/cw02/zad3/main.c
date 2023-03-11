#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>


int main(){
    DIR *dir;
    struct dirent *entry;
    struct stat sb;

    if ((dir = opendir(".")) == NULL){ //problem opening the directory
        perror("opendir");
        exit(1);
    }

    long long size = 0;

    entry = readdir(dir); //reading the directory
    while (entry != NULL){
        
        if (stat(entry->d_name, &sb) == -1){ //stat is returning a stat structure -> if not error
            perror("stat");
            exit(1);
        }
        if(!S_ISDIR(sb.st_mode)){ //if it's not a directory
            size += sb.st_size;
            printf("%s, %ld\n", entry->d_name, sb.st_size);
        }
        entry = readdir(dir);
    }

    closedir(dir);

    printf("Summary size of files: %lld bytes", size);
}