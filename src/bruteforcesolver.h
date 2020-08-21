#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>

typedef struct cell
{
    int  blk_num;
    int  value;   /* 0-9 */
    bool cb;      /* confidence bit, 0 means it is a guess */
} cell;

void loadbrd(char *file, cell board[][9], int side);
void printbrd(cell board[][9], int side); 
int solve(cell board[][9], int side, int start_x, int start_y);
int sm_val_num(int addx, int addy, int lower, cell board[][9], int side);
