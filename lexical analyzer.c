#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#define BUFSIZE 255 // цифра из задания, а не презентации. отвечает за размер буфера
#define MAXLEN BUFSIZE // макс. длина чего-либо. привязана к BUFSIZE для удобства
#define TNUMLEN 100 // длина TNUM

enum state { // состояния
    H,
    ID,
    NUM,
    COM,
    ASS,
    DLM,
    ER,
    FIN,
    AFTERBRACKET // состояние для проверки содержимого после скобки, т.к. знак комментария (* 
};

char buf[BUFSIZE]; /* для накопления символов лексемы */
int c; /* очередной символ */
int d; /* для формирования числового значения константы */
int j; /* номер строки в таблице, где находится лексема, найденная функцией look */

//массивы
// служ. слова
char TW[][MAXLEN] = { "begin", /*из задания*/ "do", "while", "loop" /**/, "and", "or", "not", "end"};
int TWlen = sizeof(TW) / MAXLEN;
// разделители
char TD[][MAXLEN] = { /* комментарии */ "(*", "*)" /**/,"*", "+", "-", "/", ":=", "!=", "=", "<", ">", ";", "(", ")"}; 
int TDlen = sizeof(TD) / MAXLEN;
// идентификаторы
char TID[MAXLEN][BUFSIZE];
int TIDlast = 0;
// числа
char TNUM[TNUMLEN];
int TNUMlast = 0;

int rowCount = 1; // номер строки на которой мы находимся
int bufLast = 0; // индекс последнего свободного в buf

// функции

void printLex(int arrID, int lineInArr) { // вывод < , >, сделана для удобства
    printf("<%d,%d>", arrID, lineInArr);
}

void printError(char str[]) { // печать сообщения об ошибке, сделана для удобства
    printf("\n-------ОШИБКА-------\n");
    printf("Срока ошибки: %d\n", rowCount);
    printf("Сообщение: %s!\n", str);
    printf("--------------------");
}

void clear(void) { /* очистка буфера buf */
    for (int i = 0; i < BUFSIZE; i++)
        buf[i] = NULL;
    bufLast = 0; // обнуляем индекс последнего свободного
}

void add(void) { /* добавление символа с в конец буфера buf */
    buf[bufLast] = c;
    bufLast++;
    return;
}

int look(char arr[][BUFSIZE], int length) { /* поиск в таблице лексемы из buf; результат: номер строки таблицы либо 0 */
    for (int i = 0; i < length; i++) {
        if (!strcmp(arr[i], buf)) // проверка схожести строк
            return ++i; // индекс строки + 1
    }
    return 0; // если не нашли
}

int putl() { /* запись в таблицу лексемы из buf, если ее там не было; результат:номер строки таблицы */
    int tempLineNum; // индекс строки, которую нашли
    if (tempLineNum = look(TID, TIDlast)) // если она не ноль
        return tempLineNum;
    for (int i = 0; buf[i] != NULL; i++)  // запись из buf до конца строки в ней
        TID[TIDlast][i] = buf[i];
    return ++TIDlast;
}

int putnum() { /* запись в TNUM константы из d, если ее там не было; результат: номер строки таблицы TNUM */
    for (int i = 0; i < TNUMlast; i++) // своя функция look, т.к. массив TNUM - одномерный
        if (d == TNUM[i]) // если найденый номер уже существует
            return ++i;
    TNUM[TNUMlast] = d;
    return ++TNUMlast;
}

void main() {
    setlocale(LC_ALL, "Russian");
    enum state TC; /* текущее состояние */
    FILE* fp; // указатель на файл
    TC = H;
    fp = fopen("prog.txt", "r"); // в файле"prog" находится текст исходной программы 
    if (!fp) {
        printError("Файл не обнаружен!");
        return;
    }
    c = fgetc(fp);
    // printf("%d  ", rowCount); // вывод номера строки
    do {
        switch (TC) {
        case H: // ожидание
            if (c == '\n') {
                c = fgetc(fp);
                rowCount++; // изменяём номер строки
                // printf("\n%d  ", rowCount); // вывод номера строки
                printf("\n"); // для отображения по строкам без номера строки
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
        case ID: // идентификатор
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
        case NUM: // число
            if (isdigit(c)) {
                d = d * 10 + (c - '0');
                c = fgetc(fp);
            }
            else {
                if (isalpha(c)) { // тут у нас в "c" след. символ, если он буква, то это идентификатор и надо выдать ошибку
                    printError("Идентификатор начинается с цифры");
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
                    if (c == ')') { // встретили *)
                        TC = H;
                        c = fgetc(fp);
                        break;
                    }
                    else  if (c == EOF) { // если комментарий не закрыт
                        TC = ER;
                        printError("Комментарий не был закрыт");
                        break;
                    }
                }
                else  if (c == EOF) { // если комментарий не закрыт
                    TC = ER;
                    printError("Комментарий не был закрыт");
                    break;
                }
            }
            break;
        case ASS: // присвоение
            if (c != '=') { // проверяем является ли это знаком присвоения, а не просто :
                printError("Недопустимый символ");
                TC = ER;
                break;
            }
            add(); // сздаём ":=" в буфере и получаем номер строки его в TD
            printLex(2, look(TD, TDlen));
            c = fgetc(fp);
            TC = H;
            break;
        case AFTERBRACKET: // То, что после скобок
            if (c == '*') // если это коментарий
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
                printError("Недопустимый символ");
            }
            break;
        }
        /* конец switch */
    } while (TC != FIN && TC != ER);

    // вывод таблиц
    int i = 0;
    printf("\nТаблицы:");
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
        printf("Пусто");
    else 
        for (i = 0; i < TIDlast; i++) {
            printf("%s  ", TID[i]);
        }
    printf("\n(4) TNUM: ");
    if (TNUMlast == 0)
        printf("Пусто");
    else 
        for (i = 0; i < TNUMlast; i++) {
            printf("%d  ", TNUM[i]);
        }

    // вывод результата анализа
    if (TC == ER)
        printf("\nERROR!!!\n");
    else
        printf("\nO.K.!!!\n");

    system("pause");
}

