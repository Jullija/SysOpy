#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string.h>

#define MAX_LENGTH 255


void showMeWhatYouGot(char* path, char* text){
    DIR* dir;
    struct dirent* entry;
    struct stat sb;

    if ((dir = opendir(path)) == NULL){
        perror("opendir");
        exit(-1);
    }

    //going inside
    while ((entry = readdir(dir)) != NULL){


        //do not go there
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
            
        }

        char filePath[PATH_MAX+1];

        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name); //formatting to filePath text as this/is/directory/to/this/entry
        //now in filePath we have a path to directory (first %s is path, /, %s is entry)

        if (stat(filePath, &sb) == -1){
            perror("stat");
            exit(-1);
        }

        //directory
        if (S_ISDIR(sb.st_mode)){

            pid_t saveFork = fork();

            if (saveFork < 0){
                perror("fork");
                exit(-1);
            }

            if (saveFork == 0){ //it's a child
                fflush(stdout); //writing buffer 
                showMeWhatYouGot(filePath, text);
                exit(EXIT_SUCCESS);

            }

        }

        //file
        else if (S_ISREG(sb.st_mode)){
            FILE* openedFile = fopen(filePath, "r");

            if (openedFile == NULL){
                perror("file");
                exit(-1);
            }

            char buffer[MAX_LENGTH + 1];
            //reading data from the file to the buffer
            size_t bookWorm = fread(buffer, sizeof(char), MAX_LENGTH, openedFile); 
            buffer[bookWorm] = '\0';

            if (strncmp(buffer, text, strlen(text)) == 0){ //comparing char by char and they're the same
                printf("Filepath: %s, PID: %d\n", filePath, getpid());
                fflush(stdout); //writing buff
            }
            fclose(openedFile);
        }


    }
    fflush(stdout);
    if (closedir(dir) == -1){
        perror("closedir");
        exit(-1);
    }
    closedir(dir);
    return;
}


int main(int argc, char** argv){
    if (argc != 3){
        fprintf(stderr, "Wrong number of arguments given");
        exit(-1);
    }

    if (strlen(argv[2]) > MAX_LENGTH){
        fprintf(stderr, "Maximum length is 255 bytes");
        exit(-1);
    }

    fflush(stdout);
    showMeWhatYouGot(argv[1], argv[2]);
    wait(NULL);
    return 0;

}