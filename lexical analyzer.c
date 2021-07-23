#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#define BUFSIZE 255 // buffer size
#define MAXLEN BUFSIZE // max length of a word
#define TNUMLEN 100 // TNUM length

enum state { // states of the program
    H,
    ID,
    NUM,
    COM,
    ASS,
    DLM,
    ER,
    FIN,
    AFTERBRACKET // check comment content 
};

// temporary
char buf[BUFSIZE]; // buffer of a word
int c; // symbol to be processed
int d; // constat
int j; // string number

// service words
char TW[][MAXLEN] = { "begin", "do", "while", "loop" , "and", "or", "not", "end"};
int TWlen = sizeof(TW) / MAXLEN;
// delimeters
char TD[][MAXLEN] = { "(*", "*)","*", "+", "-", "/", ":=", "!=", "=", "<", ">", ";", "(", ")"}; 
int TDlen = sizeof(TD) / MAXLEN;
// identficators
char TID[MAXLEN][BUFSIZE];
int TIDlast = 0;
// numbers
char TNUM[TNUMLEN];
int TNUMlast = 0;

int rowCount = 1; // current row
int bufLast = 0; // index of free space in buf

// functions
void printLex(int arrID, int lineInArr) {
    printf("<%d,%d>", arrID, lineInArr);
}

void printError(char str[]) { // error message
    printf("\n-------ERROR-------\n");
    printf("ROW: %d\n", rowCount);
    printf("MESSAGE: %s!\n", str);
    printf("--------------------");
}

void clear(void) { // clear buf
    for (int i = 0; i < BUFSIZE; i++)
        buf[i] = NULL;
    bufLast = 0;
}

void add(void) { // add symbol to bug
    buf[bufLast] = c;
    bufLast++;
    return;
}

int look(char arr[][BUFSIZE], int length) { // search word in the array. Result either 0 or index of the word
    for (int i = 0; i < length; i++) {
        if (!strcmp(arr[i], buf)) // check if similiar
            return ++i;
    }
    return 0; // not found
}

int putl() { // write word to the array. result is the index of the word in the array
    int tempLineNum; 
    if (tempLineNum = look(TID, TIDlast))
        return tempLineNum;
    for (int i = 0; buf[i] != NULL; i++)
        TID[TIDlast][i] = buf[i];
    return ++TIDlast;
}

int putnum() { // write constatnt (d) in TNUM. result: index of the constant in array
    for (int i = 0; i < TNUMlast; i++) // cant use look as the array is one-dimensional
        if (d == TNUM[i])
            return ++i;
    TNUM[TNUMlast] = d;
    return ++TNUMlast;
}

void main() {
    // setlocale(LC_ALL, "Russian");
    enum state TC; // current state
    FILE* fp; // указатель на файл
    TC = H;
    fp = fopen("prog.txt", "r"); // file of the program
    if (!fp) {
        printError("Файл не обнаружен!");
        return;
    }
    c = fgetc(fp);
    // printf("%d  ", rowCount); // show row number
    do {
        switch (TC) {
        case H: // wait state
            if (c == '\n') {
                c = fgetc(fp);
                rowCount++; // change row number
                // printf("\n%d  ", rowCount); // show row number
                printf("\n");
                break;
            }
            if (c == ' ')
                c = fgetc(fp);
            else
                if (isalpha(c)) {
                    clear();
                    add();
                    c = fgetc(fp);
                    TC = ID;
                }
                else if (isdigit(c)) {
                    d = c - '0';
                    c = fgetc(fp);
                    TC = NUM;
                }
                else if (c == '(') {
                    clear();
                    add();
                    c = fgetc(fp);
                    TC = AFTERBRACKET;
                }
                else if (c == ':') {
                    clear();
                    add();
                    c = fgetc(fp);
                    TC = ASS;
                }
                else if (c == EOF) {
                    TC = FIN;
                }
                else TC = DLM;
            break;
        case ID: // identificator
            if (isalpha(c) || isdigit(c)) {
                add();
                c = fgetc(fp);
            }
            else {
                j = look(TW, TWlen);
                if (j)
                    printLex(1, j);
                else {
                    j = putl();
                    printLex(3, j);
                };
                TC = H;
            };
            break;
        case NUM: // number
            if (isdigit(c)) {
                d = d * 10 + (c - '0');
                c = fgetc(fp);
            }
            else {
                if (isalpha(c)) { // c is the next sumbol. if it is a number, show error
                    printError("ID starts with letter!");
                    TC = ER;
                    break;
                }
                printLex(4, putnum());
                TC = H;
            }
            break;
        case COM: // комментарий
            while (1) {
                c = fgetc(fp);
                if (c == '*') {
                    c = fgetc(fp);
                    if (c == ')') {
                        TC = H;
                        c = fgetc(fp);
                        break;
                    }
                    else  if (c == EOF) {
                        TC = ER;
                        printError("Comment section was not closed");
                        break;
                    }
                }
                else  if (c == EOF) {
                    TC = ER;
                    printError("Comment section was not closed");
                    break;
                }
            }
            break;
        case ASS: // assignment
            if (c != '=') { // check if this is :=
                printError("Unknown character!");
                TC = ER;
                break;
            }
            add(); // add :=
            printLex(2, look(TD, TDlen));
            c = fgetc(fp);
            TC = H;
            break;
        case AFTERBRACKET:
            if (c == '*') // check if this is the comment
                TC = COM;
            else {
                j = look(TD, TDlen);
                TC = H;
                printLex(2, j);
            }
            break;
        case DLM:  
            clear();
            add();
            j = look(TD, TDlen);
            if (j) {
                c = fgetc(fp);
                printLex(2, j);
                TC = H;
            }
            else {
                TC = ER;
                printError("Unknown character!");
            }
            break;
        }
        
    } while (TC != FIN && TC != ER);

    // show table
    int i = 0;
    printf("\nArrays:");
    printf("\n(1) TW: ");
    for (; i < TWlen; i++) {
        printf("%s  ", TW[i]);
    }
    printf("\n(2) TD: ");
    for (i = 0; i < TDlen; i++) {
        printf("%s  ", TD[i]);
    }
    printf("\n(3) TID: ");
    if (TIDlast == 0)
        printf("Empty");
    else 
        for (i = 0; i < TIDlast; i++) {
            printf("%s  ", TID[i]);
        }
    printf("\n(4) TNUM: ");
    if (TNUMlast == 0)
        printf("Empty");
    else 
        for (i = 0; i < TNUMlast; i++) {
            printf("%d  ", TNUM[i]);
        }

    // вывод результата анализа
    if (TC == ER)
        printf("\nERROR!\n");
    else
        printf("\nValidated successfully\n");

    system("pause");
}

