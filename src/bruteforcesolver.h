#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

typedef struct point
{
    int blk_num;
    int value;
    int cb; /* confidence bit, 0 means it is a guess */
} point;

void loadbrd(char *file, point board[][9], int side);
void printbrd(point board[][9], int side); 
int solve(point board[][9], int side, int start_x, int start_y);
int sm_val_num(int addx, int addy, int lower, point board[][9], int side);
