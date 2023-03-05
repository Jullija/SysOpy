#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct pointerTable{
    char ** table;
    int maxSize;
    int currSize;
} pointerTable;

pointerTable * createPointerTable(int size);
void fillPointerTable(pointerTable * table, char filename[]);
char * contentOnIndex(pointerTable * table, int index);
void deleteOnIndex(pointerTable * table, int index);
void deleteAll(pointerTable * table);

#endif