#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define Warning(Str)      fprintf(stderr, "%s", Str)
#define Error(Str)        FatalError(Str)
#define FatalError(Str)   fprintf(stderr, "%s", Str), exit(1)

// TODO: try whether this error message works with arguments
//#define Error2(Str, ...)    fprintf(stderr, args)

// Color Text
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef struct point
{
//    int value;
    int row;
    int col;
    int blk;
    uint16_t bit_vector; 	/* 	9-bit vector, using bit 1 to 9, 
                                bit i == 0 means i is a possible value 
                                NOTE possible value starts from 1 instead of 0
                                16   10|9           1|0
                                xxxxxxx|<---{0,1}--->|x
                                bit 0 == 0 represent whether it is unknown initially
                                vice versa                    						*/
} point;

#ifndef _DynamicSolver_h

    void loadbrd(char *file);
    void printbrd();
    void updatebrd();

    int unique_soln(uint16_t bv);
    int get_value(uint16_t bv);
	void mask_value(point *pt, int val);
	void update_pt(point *pt, int val);
    int hamming_count(uint16_t x);
    void step();
    void step2();

#endif  /* _DynamicSolver_h */
