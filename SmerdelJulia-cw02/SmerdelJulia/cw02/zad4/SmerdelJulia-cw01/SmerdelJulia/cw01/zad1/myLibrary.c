#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 1000

typedef struct pointerTable
{
    char **table; 
    int maxSize;
    int currSize;
} pointerTable;

pointerTable * createPointerTable(int size){
    if (size <= 0)
        {
            exit(-1);
        }

    pointerTable * table = malloc(sizeof(pointerTable)); //necessary for dynamic library, since it's operating on pointers

    table->table = calloc(size, sizeof(char *));
    table->currSize = 0;
    table->maxSize = size;

    return table;
}


//I'm saving words to the table (I might misunderstood the command). To save the information given by "wc" command -> commented part
void fillPointerTable(pointerTable *table, char filename[])
{

    if (strlen(filename) > 0 && filename[strlen(filename)-1] == '\n'){
        filename[strlen(filename)-1] = '\0';
    }

    const char *firstCommand = "wc ";
    const char *secondCommand = " > /tmp/tempFile.txt";
    char *fullCommand = malloc(strlen(firstCommand) + strlen(secondCommand) + strlen(filename) + 1);

    //creating a command wc filename > tmp/tempFile.txt -> saving data to tempFile.txt file
    strcpy(fullCommand, firstCommand); 
    strcat(fullCommand, filename);
    strcat(fullCommand, secondCommand);
    system(fullCommand);
    free(fullCommand);

    //reading text from file
    FILE *f;
    int numWords, numLines, numBytes;
    f = fopen("/tmp/tempFile.txt", "r");
    fscanf(f, "%d %d %d", &numLines, &numWords, &numBytes);
    printf("number of words in file: %d\n", numWords);
    fclose(f);
    remove("/tmp/tempFile.txt");

    if (numWords > table->maxSize - table->currSize)
    {
        fprintf(stderr, "There is no enough free space in table");
        exit(3);
    }


    FILE * f2;
    f2 = fopen(filename, "r");

    if (f2 == NULL)
    {
        fprintf(stderr, "Error opening file.\n");
        exit(1);
    }
    
    char word[MAX_WORD_LENGTH];

    while (fscanf(f2, "%s", word) == 1)
    {
        table->table[table->currSize] = malloc((strlen(word) + 1) * sizeof(char));
        strcpy(table->table[table->currSize], word);
        table->currSize++;
    }

    fclose(f2);


    
    //To save the information given by the "wc" command (numLines, numWords, numBytes)
    // FILE *f;
    // int numWords, numLines, numBytes;
    // f = fopen("/tmp/tempFile.txt", "r");


    // if (3 > table->maxSize - table->currSize)
    // {
    //     fprintf(stderr, "There is no enough free space in table");
    //     exit(3);
    // }

    // char data[MAX_WORD_LENGTH];

    // f = fopen("/tmp/tempFile.txt", "r");
    // int iter = 0;
    // while (fscanf(f, "%s", data) == 1 && iter < 3)
    // {
    //     table->table[table->currSize] = malloc((strlen(data) + 1) * sizeof(char));
    //     strcpy(table->table[table->currSize], data);
    //     table->currSize++;
    //     iter++;
    // }

    // fclose(f);
    // remove("/tmp/tempFile.txt");
 
}

char *contentOnIndex(pointerTable *table, int index)
{
    if (index >= table->currSize || index < 0)
    {
        fprintf(stderr, "Error invalid index value.\n");
        exit(2);
    }
    return table->table[index];
}

void deleteOnIndex(pointerTable *table, int index)
{
    if (index >= table->currSize || index < 0)
    {
        fprintf(stderr, "Error invalid index value.\n");
        exit(2);
    }

    free(table->table[index]);
    table->table[index] = NULL;
    table->currSize--;
    for (int i = index; i < table->currSize; i++)
    {
        table->table[i] = table->table[i + 1];
    }
    table->table[table->currSize] = NULL;
}

void deleteAll(pointerTable *table)
{
    for (int i = 0; i < table->maxSize; i++)
    {
        if (table->table[i] != NULL){
            free(table->table[i]);
        }
    }
    if (table->table){
        free(table->table);
    }
}

