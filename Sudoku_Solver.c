

#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

#define N 9

unsigned int flag = false;

void printSudokuBoard(int (*board)[N][N]) { // function for printing sudoku board
    printf("\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++) {
            printf("%d ", (*board)[i][j]);
            if (((j + 1) % 3) == 0)
                printf("  ");
        }
        printf("\n");
        if (((i + 1) % 3) == 0)
            printf("\n");
    }
}

int isDone( int (*board)[N][N] ) { // function checks if the board is solved (no zeroes)
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 9; y++)
            if ((*board)[x][y] == 0)
                return false;
    return true;
}

int* getPossibleNumbersArr(int (*board)[N][N], int x, int y) { // function for making an array of numbers that fits in board[x][y] place ( board[x][y] == 0 )

    int* numArr; // pointer to the array we need to make
    if (!(numArr = (int*)malloc(sizeof(int) * N))) // make array out of N elements (0..9) -> (1..9) 
                                                   // numArr[i] means whether i + 1 is possible number for that position
        return NULL; // memory allocation error
    //int numArr[N];

    for (int i = 0; i < N; i++)
        numArr[i] = true; // all the numbers are possible

    //  for numbers in horizontal direction

    for (int i = 0; i < N; i++)
        if ((*board)[x][i] != 0) 
            numArr[(*board)[x][i] - 1] = false; // if there is a number != 0 in horizontal direction, it can't be used

    //  for numbers in vertical direction

    for (int j = 0; j < N; j++) 
        if ((*board)[j][y] != 0)
            numArr[(*board)[j][y] - 1] = false; // if there is a number != 0 in vertical direction, it can't be used


    //  for numbers in a square 3x3, that this number is placed at

    // get a in ... a b c ... ( first element of a squeare this number is placed at )
    //          ... d e f ...
    //          ... g h e ...

    int sq_coord_x, sq_coord_y;

    if ((x >= 0) && (x <= 2))
        sq_coord_x = 0;
    else if ((x >= 3) && (x <= 5))
        sq_coord_x = 3;
    else
        sq_coord_x = 6;

    if ((y >= 0) && (y <= 2))
        sq_coord_y = 0;
    else if ((y >= 3) && (y <= 5))
        sq_coord_y = 3;
    else
        sq_coord_y = 6;

    // for square

    for (int i = sq_coord_x; i < sq_coord_x + 3; i++)
        for (int j = sq_coord_y; j < sq_coord_y + 3; j++)
            if ((*board)[i][j] != 0)
                numArr[(*board)[i][j] - 1] = false;

  // end checking


    for (int i = 0; i < N; i++) // convert array to array of numbers and zeroes
        if (numArr[i] == true)
            numArr[i] = i + 1;


    return numArr; // output array of possible numbers
}

void getEmptySpot(int(*board)[N][N], int* x, int* y) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if ((*board)[i][j] == 0) {
                *x = i;
                *y = j;
                return;
            }
}

void solve( int (*board)[N][N] ) { // recursive function for solving the sudoku board


    //# recursive function which solved the boardand
    //# prints it.

    if (isDone(board)) { // check if there is no possible points
        printf("Board Solved Successfully!");
        printSudokuBoard(board);
        flag = true;
        return;
    }

    int x, y; // (x;y) coords of the point
    int* valsArr = NULL; // pointer for an array of possible numbers

    getEmptySpot(board, &x, &y); // find empty spot

    valsArr = getPossibleNumbersArr(board, x, y); // get possible values array

    for (int i = 0; i < N; i++)
        if (valsArr[i] != 0) {
            (*board)[x][y] = valsArr[i]; // makes board[x][y] one of the possible numbers
            solve(board); // recursion
            // backtrack
            (*board)[x][y] = 0; // make it back zero
        }
}

void getValues(int(*board)[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            scanf_s("%d", &((*board)[i][j]));
}

void main() {

    int sud_board[N][N]; // sudoku board | 0 = empty place

    // exaple: 0 0 0 3 0 0 2 0 0 0 0 0 0 0 8 0 0 0 0 7 8 0 6 0 3 4 0 0 4 2 5 1 0 0 0 0 1 0 6 0 0 0 4 0 9 0 0 0 0 8 6 1 5 0 0 3 5 0 9 0 7 6 0 0 0 0 7 0 0 0 0 0 0 0 9 0 0 5 0 0 0
    getValues(&sud_board);

    solve(&sud_board);

    if (!flag)
        printf("No solutions for the sudoku board!");
}



