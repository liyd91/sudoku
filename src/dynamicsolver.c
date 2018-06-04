/* ======================================================================================
 *
 *          Dynamic Sudoku Solver
 *
 *//*  @file dynamicsolver.c
 *  This file contains the implementation of dynamic sudoku solver
 *
 *  @par EXTERNALIZED FUNCTIONS
 *      (NONE)
 *
 *  @par INITIALIZATION AND SEQUENCING REQUIREMENTS
 *      (NONE)
 *
 *   by Yandong Li   @ 06/30/2013
 *       Computer Engineering
 *   University of Illinois at Urbana-Champaign
 *   Urbana, IL, USA
 *
 *//*================================================================================= */

#include "dynamicsolver.h"

// the sudoku board has dimension of 9 x 9
point board[81];

/*

    0 1 2  3 4 5  6 7 8    
    ----------------------
0   |      |      |      |
1   | blk0 | blk1 | blk2 |
2   |      |      |      |
    ----------------------
3   |      |      |      |
4   | blk3 | blk4 | blk5 |
5   |      |      |      |
    ----------------------
6   |      |      |      |
7   | blk6 | blk7 | blk8 |
8   |      |      |      |
    ----------------------

*/

int blk[9][9] = {   {0,1,2,9,10,11,18,19,20},
                    {3,4,5,12,13,14,21,22,23},
                    {6,7,8,15,16,17,24,25,26},
                    {27,28,29,36,37,38,45,46,47},
                    {30,31,32,39,40,41,48,49,50},
                    {33,34,35,42,43,44,51,52,53},
                    {54,55,56,63,64,65,72,73,74},
                    {57,58,59,66,67,68,75,76,77},
                    {60,61,62,69,70,71,78,79,80}    };

/* ---------------------------------------------------------------------------
 *    FUNCTION        main
 *    DESCRIPTION     
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
int main(int argc, char * argv[])
{
    clock_t start, end;
    double runtime;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [input_file]\n", argv[0]);
        return EXIT_FAILURE;
    }
	
    start = clock();
    
    loadbrd(argv[1]);
    
    printbrd();

//    initial_step();
    updatebrd();

    printbrd();
    
    updatebrd();
    
    printbrd();
    
    step();
    
    printbrd();
    
    step();
    
    printbrd();
   	
    
    // stop timer
    end = clock();
    runtime = ((end-start) / (double) CLOCKS_PER_SEC );
    printf("Run time is %g seconds\n", runtime);
    
    return EXIT_SUCCESS;
}

/* ---------------------------------------------------------------------------
 *    FUNCTION        loadbrd
 *    DESCRIPTION     
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
void loadbrd(char *file)
/* read the board in the row major order */
{
    int row, col, ro, co;
    char temp;
    int tmp_digit;
	
    // integer conversion error checking
    char *end;
    long sl;
    
    FILE *fpt;
    fpt = fopen(file, "r");
    
    if (fpt == NULL)
        Error("ERROR: FILE NOT FOUND!\n");
    
    // load data from rough data map
    for (row = 0; row < 9; row++){
   	
        for (col = 0; col < 9; col++){
        
            temp = fgetc(fpt);
    
            // valid input is only digit 1 - 9 and '-'
            
            if (temp == '-')
            {
//                board[row*9+col].value = 0;
                board[row*9+col].bit_vector = 0x03FE; // 0x0000 0011 1111 1110
            }
            else
            {
                // error check for string conversion
                sl = strtol(&temp, &end, 10);
                if (end == &temp)
                {
                    fprintf(stderr, "%s: not a decimal number\n", &temp);
                    exit (1);
                }
                else
                {
                    tmp_digit = (int)sl;
                }
                
                // the least significant bit returns 1 not 0 in ffs()
//                board[row*9+col].value = tmp_digit;
                board[row*9+col].bit_vector = (1 << tmp_digit) | 0x0001;
            }

            board[row*9+col].row = row;
            board[row*9+col].col = col;

            if (row >= 0 && row < 3) ro = 0;
            else if (row >= 3 && row < 6) ro = 1;
            else if (row >= 6 && row < 9) ro = 2;
            if (col >= 0 && col < 3) co = 0;
            else if (col >= 3 && col < 6) co = 1;
            else if (col >= 6 && col < 9) co = 2;
            board[row*9+col].blk = ro * 3 + co;
        }
        // get the line carriage
        fgetc(fpt);
    }
    fclose(fpt);
}

/* ---------------------------------------------------------------------------
 *    FUNCTION        printbrd
 *    DESCRIPTION     
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
void printbrd()
{
    int row, col;
    for (row = 0; row < 9; row++){

        if (((row % 3) == 0) && (row != 0)) 
            printf(" ---------------------\n");
            
        for (col = 0; col < 9; col++){
        
            if (((col % 3) == 0) && (col != 0)) 
                printf(" %s|", KWHT);
            
            if (board[row*9+col].bit_vector & 0x0001)
            {
            	printf(" %s%d", KWHT, get_value(board[row*9+col].bit_vector));
            }
            else
            {
            	if ( unique_soln(board[row*9+col].bit_vector & 0xFFFE) )
            	{
            		printf(" %s%d", KGRN, get_value(board[row*9+col].bit_vector));
            	}
            	else
            	{
            	    printf(" %s%d", KRED, 0);
            	}
            }
        }
        printf("%s\n", KWHT);
    }
    
    printf( "----------------------------------\n"
            "----------------------------------\n");
}

// being what it has to be
//
// e.g. in a row like 9 8 _ 6 5 _ 3 2 _,
// if the first blank could be 4 or 7
// and if the second could be  1 or 4
// and if the third could be   1 or 4
// then the first blank has to be 7 
// NOTE: failed at brd7.txt, some point overwritten
void step()
{
    int row, col, blk_num;
    int scan, i, j;
    point * temp[9];
    int temp_count = 0;
    uint16_t tmp;

    // check every row
    for (row = 0; row < 9; row++)
    {
        temp_count = 0;

        // scan through the row to find the unknown slots
        for (scan = 0; scan < 9; scan++)
        {
            if (!unique_soln(board[row*9+scan].bit_vector))
            {
                temp[temp_count] = &(board[row*9+scan]);
                temp_count++;
            }
        }
        if (temp_count == 0)
            continue;

        for (i = 1; i <= 9; i++)
        {
            tmp = 0x0000;
            
            for (j = 0; j < temp_count; j++)
            {
                tmp |= ((temp[j]->bit_vector & (0x0001 << i)) >> i) << j;
            }

            // check the value is the only available in the bit-vector
            if (hamming_count(tmp) == 1)
            	update_pt(temp[ffs(tmp)-1], i);
        }
    } // check every row 
	
    // check every column
    for (col = 0; col < 9; col++)
    {
        temp_count = 0;

        // scan through the col to find the unknown slots
        for (scan = 0; scan < 9; scan++)
        {
            if (!unique_soln(board[scan*9+col].bit_vector))
            {
                temp[temp_count] = &(board[scan*9+col]);
                temp_count++;
            }
        }
        if (temp_count == 0) 
            continue;

        for (i = 1; i <= 9; i++)
        {
            tmp = 0x0000;
            
            for (j = 0; j < temp_count; j++)
            {
                tmp |= ((temp[j]->bit_vector & (0x0001 << i)) >> i) << j;
            }

            // check the value is the only available in the bit-vector
            if (hamming_count(tmp) == 1)
            	update_pt(temp[ffs(tmp)-1], i);
        }
    } // check every column

    // check every block
    for (blk_num = 0; blk_num < 9; blk_num++)
    {
        temp_count = 0;

        // scan through the block to find the unknown slots
        for (scan = 0; scan < 9; scan++)
        {
            if (!unique_soln(board[blk[blk_num][scan]].bit_vector))
            {
                temp[temp_count] = &(board[blk[blk_num][scan]]);
                temp_count++;
            }
        }
        if (temp_count == 0) 
            continue;

        for (i = 1; i <= 9; i++)
        {
            tmp = 0x0000;
            
            for (j = 0; j < temp_count; j++)
            {
                tmp |= ((temp[j]->bit_vector & (0x0001 << i)) >> i) << j;
            }

            // check the value is the only available in the bit-vector
            if (hamming_count(tmp) == 1)
            	update_pt(temp[ffs(tmp)-1], i);
        }
    } // check every block
} /* step */

// row/col in a block contains a value
void step2()
{
    int row, col, blk_num;

    for (blk_num = 0; blk_num < 9; blk_num++)
    {
        col = blk[blk_num][0] % 9;
        row = (blk[blk_num][0] - col) / 9;

    }

} /* step2 */

/* ---------------------------------------------------------------------------
 *    FUNCTION        updatebrd
 *    DESCRIPTION     
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
void updatebrd()
{
	int row, col;
    
    for (row = 0; row < 9; row++)
    {
        for (col = 0; col < 9; col++)
        {
            if (board[row*9+col].bit_vector == 0)
            {
                fprintf(stderr, "(%d,%d) has an invalid bit vector\n", row, col);
                exit (1);
            }

            if ( unique_soln(board[row*9+col].bit_vector & 0x03FE) )
            	update_pt(&board[row*9+col], get_value(board[row*9+col].bit_vector));
        }
    }
}

/* ---------------------------------------------------------------------------
 *    FUNCTION        update_pt
 *    DESCRIPTION     updates the bit_vectors of related points
 *                    related points, are points in the same row/col/blk
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
void update_pt(point *pt, int val)
{
	int row, col, blk_num;
	int scan;
	uint16_t bv;
	
	if (pt == NULL)
    { 
        Warning("WARNING: Invalid Parameter (point *pt)\n");
        return;
    }

	row = pt->row;
	col = pt->col;
	blk_num = pt->blk;
    bv = pt->bit_vector;
	
	if (val > 9 || val < 1)
    {
        Warning("WARNING: Invalid Parameter (int val), out of range\n");
        return;
    }
	
    if ( !(pt->bit_vector & (1 << val)) )
    {
        Warning("WARNING: Improper operation\n");
        return;
    }

//	pt->value = val;
	pt->bit_vector &= ((1 << val) | 0xFC01);
	
	for (scan = 0; scan < 9; scan++)
	{
        // update points on the same row
    	if (scan != col)
    		mask_value(&board[row*9+scan], val);
        // update points on the same column
        if (scan != row)
            mask_value(&board[scan*9+col], val);
        // update points in the same block
        if (board[blk[blk_num][scan]].row != row ||
            board[blk[blk_num][scan]].col != col)
        	mask_value(&board[blk[blk_num][scan]], val);
	}
} /* update_pt */

/* ---------------------------------------------------------------------------
 *    FUNCTION        hamming_count
 *    DESCRIPTION     counts the number of set bits
 *    DEPENDENCIES    
 *    PARAMETERS      
 *    RETURN VALUE    
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
int hamming_count(uint16_t x)
{
    // EXAMPLES:
    // 0000 000 0 0000 1000 should return 1
    // 0000 000 1 0001 0110 should return 4
    // 0000 000 0 1010 1011 should return 5

    x -= (x >> 1) & 0x5555;                 //put count of each 2 bits into those 2 bits
    x = (x & 0x3333) + ((x >> 2) & 0x3333); //put count of each 4 bits into those 4 bits
    x += x >> 4;
    x += x >> 8;

    // at max the count would return 9 (4 bit would be enough)
    return x & 0x000F;
}

//This uses fewer arithmetic operations than any other known  
//implementation on machines with slow multiplication.
//It uses 17 arithmetic operations.
/*int popcount_2(uint64_t x) {
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    x += x >>  8;  //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;  //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;  //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
} */  

/* ---------------------------------------------------------------------------
 *    FUNCTION        unique_soln
 *    DESCRIPTION     
 *    DEPENDENCIES    ONLY valid for non-initially-set points
 *    PARAMETERS      
 *    RETURN VALUE    boolean value, 
 *                    TRUE iff bit vector contains only 1 set bit
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
int unique_soln(uint16_t bv)
{
    return ((bv & (bv-1)) == 0);
}

/* ---------------------------------------------------------------------------
 *    FUNCTION        get_value
 *    DESCRIPTION     gets the value according to a bit_vector,
 *                    valid for initially-set points
 *    DEPENDENCIES    MUST call unique_soln to check uniqueness before
 *    PARAMETERS      
 *    RETURN VALUE    the position of the unique bit in bit field 1 - 9
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
int get_value(uint16_t bv)
{
    // the least significant bit returns 1 not 0 in ffs()
    return ffs(bv & 0x03FE) - 1;
}

/* ---------------------------------------------------------------------------
 *    FUNCTION        mask_value
 *    DESCRIPTION     masks the specified value in the bit vector of the point
 *    DEPENDENCIES    the function won't take effect on initially-set points
 *    PARAMETERS      
 *    RETURN VALUE    NONE
 *    SIDE EFFECTS    NONE
 * ------------------------------------------------------------------------ */
void mask_value(point *pt, int val)
{
	if (pt == NULL)
    { 
        Warning("WARNING: Invalid Parameter (point *pt)\n");
        return;
    }
    
    if ((pt->bit_vector & 0x0001) == 0)
    {
        pt->bit_vector &= ~(1 << val);
    }
}

// END dynamicsolver.c

