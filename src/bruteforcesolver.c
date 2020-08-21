/*********************************************************************************
*   a sudoku solver program                                                      *
*    recursively find the smallest valid number at each blank place              *
*    until under one situation it meets all the requirement                      *
*                                                                                *
*    by Yandong Li   @ 07/10/2011                                                *
*       Computer Engineering                                                     *
*    University of Illinois at Urbana-Champaign                                  *
*    Urbana, IL, USA                                                             *
*********************************************************************************/

#include "bruteforcesolver.h"

int main(int argc, char * argv[])
{
    clock_t start, end;
    double runtime;
    int en;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [input_file]\n", argv[0]);
        return 1;
    }

    start = clock();

    cell board[9][9];

    loadbrd(argv[1], board, 9);
    printbrd(board, 9);
    printf("----------------------------------\n"
           "----------------------------------\n");
    en = solve(board, 9, 0, 0);
    if (en == 1) {
        printf("solved:\n");
    } else {
        printf("end at:\n");
    }
    printbrd(board, 9);

    end = clock();
    runtime = ((end-start) / (double) CLOCKS_PER_SEC );
    printf("Run time is %g seconds\n", runtime);
    return 0;
}

void loadbrd(char * file, cell board[][9], int side)
/* read the board in the row major order */
{
    int row, col, ro, co;
    char temp;
    FILE *fpt;
    fpt = fopen(file, "r");
    if (fpt == NULL) {
        printf("ERROR: FILE NOT FOUND!\n");
    }

    for (row = 0; row < side; row++)
    {
        for (col = 0; col < side; col++)
        {
            temp = fgetc(fpt);
            if (temp == '-') {
                board[row][col].value = 0;
                board[row][col].cb = 0;
            }
            else {
                board[row][col].value = atoi(&temp);
                board[row][col].cb = 1;
            }

            ro = row / 3;
            co = col / 3;
            board[row][col].blk_num = ro * 3 + co;
        }
        fgetc(fpt);
    }
    fclose(fpt);
}

void printbrd(cell board[][9], int side)
{
    int row, col;
    for (row = 0; row < side; row++)
    {
        if (((row % 3) == 0) && (row != 0)) {
            printf(" ---------------------\n");
        }
        for (col = 0; col < side; col++)
        {
            if (((col % 3) == 0) && (col != 0)) {
                printf(" |");
            }
            printf(" %d", board[row][col].value);
        }
        printf("\n");
    }
}

int sm_val_num(int addx, int addy, int lower, cell board[][9], int side)
/* get the smallest valid number for a specific position (addx, addy), an originally empty cell(.cb = 0),
    if no valid number, then output 0; else, output the smallest valid number
*/
{
    int i = lower + 1, row, col, oc;
    while (i <= 9)
    {
        oc = 0; // no conflict
        for (row = 0; row < side; row++)
        {
            if (row == addx) continue;
            if (board[row][addy].value == i) oc = 1;
        }
        for (col = 0; col < side; col++)
        {
            if (col == addy) continue;
            if (board[addx][col].value == i) oc = 1;
        }
        for (row = 0; row < 9; row++)
        {
            for (col = 0; col < 9; col++)
            {
                if (board[row][col].blk_num != board[addx][addy].blk_num) continue;
                if (board[row][col].value == i) oc = 1;
            }
        }
        if (oc == 1) i++;
        else if (oc == 0) break;
    }
    if (i > 9) return 0;
    else return i;
}

int solve(cell board[][9], int side, int start_x, int start_y)
/* return 1 when go to end, else if the brunch failed, return 0 */
{
    int en = 0;
    while (!en)
    {
        printf("-----%d-----%d------------------\n", start_x, start_y);
        printbrd(board, 9);

        if (start_x == 9 && start_y == 0) return 1;

        if (board[start_x][start_y].cb)
        {
            if (start_y == 8) {
              en = solve(board, side, start_x+1, 0);
            } else {
              en = solve(board, side, start_x, start_y+1);
            }
            return en;
        }

        else
        {
            board[start_x][start_y].value = sm_val_num(start_x, start_y, 
                                                board[start_x][start_y].value, board, side);
            printf("[%d,%d] next value should be %d\n", start_x, start_y, board[start_x][start_y].value);
            if (board[start_x][start_y].value == 0) {
              return 0;
            } else {
                if (start_y == 8) {
                    en = solve(board, side, start_x+1, 0);
                } else {
                    en = solve(board, side, start_x, start_y+1);
                }
            }
        }
    }
    return en;
}
