#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

const int grid_width = 30;
const int grid_height = 30;

typedef struct{
    char* src;
    char* dst;
    int x;
    int y;
}DataContainer;



char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void* threadsFun(void* avocado)
{
    DataContainer* args = (DataContainer*) avocado;

    while (1){
        pause();

        int x = args->x;
        int y = args->y;

        args->dst[y*grid_width + x] = is_alive(y, x, args->src);

        char* tmp = args->src;
        args->src = args->dst;
        args->dst = tmp; 
    }
}

pthread_t* initThreads(char* src, char* dst){
    pthread_t* threads = malloc(sizeof(pthread_t) * grid_height * grid_width);

    int index = 0;
    for (int i = 0; i < grid_width; i++){
        for (int j = 0; j < grid_height; j++){

            DataContainer* args = malloc(sizeof(DataContainer));
            args->src = src;
            args->dst = dst;
            args->x = i;
            args->y = j;

            if (pthread_create(&threads[index], NULL, &threadsFun, args) != 0){
                printf("Error creating thread, calling grandma");
                exit(-1);
            }
            index++;
        }
    }

    return threads;
}

void IWantToBreakFree(){} //only to break the pause


void threadsUpdate(pthread_t* threads){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = IWantToBreakFree;
    sigaction(SIGUSR1, &sa, NULL);

    int threadsNum = grid_height * grid_width;
    for (int i = 0; i < threadsNum; i++){
        pthread_kill(threads[i], SIGUSR1);
    }

}

