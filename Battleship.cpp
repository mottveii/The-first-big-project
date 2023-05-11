/*Задание: написать игру морской бой. Правила стандартные.

Реализовано: игра против компьютера/для двоих игроков, автоматическая и ручная расстановки кораблей (расстановка компьютера всегда автоматическая),
алгоритм поиска и добивания для компьютера, выделение ячеек вокруг потопленных кораблей (и у компьютера и у игроков),
предотвращение некорректного ввода игроком*/

#include <iostream>
#include <ctime>
#include <conio.h>
#include <string.h>
#include <Windows.h>
using namespace std;

//размер массива хранящего данные о поле
const int SIZZE = 10;
int tempX, tempY; //для хранения координат последнего выстрела ИИ

//объявляем перечисления для удобства и повышения читабельности программы
enum Condition { EMPTY = 0, HIT, KILL, MISS, SHIP, ODSHIP, TDSHIP, THDSHIP, FDSHIP, COMPNOTSHOOT }; //состояние клетки на игровом поле
enum Moment { INIT = 0, VSCOMP, VSPLAYER, STEPP1VP2, STEPP2VP1, STEPPLAYERVC, STEPCOMP, ENDGAME }; //обозначение игрового этапа
enum Ships { ONEDECK = 1, TWODECK, THREEDECK, FOURDECK }; //обозначение кораблей с N палуб
enum Orientation { HORIZ, VERT }; //ориентация кораблей
enum Storony { RIGHT, LEFT, UP, DOWN, DONE, NDONE }; // куда смотрит нос корабля
enum II { SEARCH, DIRFIND, FINISHU, FINISHD, FINISHR, FINISHL }; //для переключения режимов добивания

//массивы, для хранения, обработки и отображения игровых данных. Такое количество экранов обусловлено возможностью игры двух людей на одном компьютере
int player[SIZZE][SIZZE] = { EMPTY }, comp[SIZZE][SIZZE] = { EMPTY }, playerscr[SIZZE][SIZZE] = { EMPTY }, compscr[SIZZE][SIZZE] = { EMPTY }, plenemyscr[SIZZE][SIZZE] = { EMPTY }, coenemyscr[SIZZE][SIZZE] = { EMPTY };

//также для удобства инициализируем массив с символами, которые будут отображать состояние клетки на игровом поле
const char wdraw[5] = { ' ', //empty
'+', //hit
'#', //kill
'o', //miss
char(219)
};

//игровое поле из нескольких строк
const char SPACE1[] = { "***ABCDEFGHIJ**************ABCDEFGHIJ" };
const char SPACE2[] = { "**------------************------------" };
char SPACE3[] = { "*1|          |***********1|          |" };
char SPACE4[] = { "*2|          |***********2|          |" };
char SPACE5[] = { "*3|          |***********3|          |" };
char SPACE6[] = { "*4|          |***********4|          |" };
char SPACE7[] = { "*5|          |***********5|          |" };
char SPACE8[] = { "*6|          |***********6|          |" };
char SPACE9[] = { "*7|          |***********7|          |" };
char SPACE10[] = { "*8|          |***********8|          |" };
char SPACE11[] = { "*9|          |***********9|          |" };
char SPACE12[] = { "10|          |**********10|          |" };
const char SPACE13[] = { "**------------************------------" };

//инициализируем указатели на первые пустые клетки вышеуказанного игрового поля для главного игрока
char* MPstr1 = strchr(SPACE3, 32), * MPstr2 = strchr(SPACE4, 32), * MPstr3 = strchr(SPACE5, 32), * MPstr4 = strchr(SPACE6, 32), * MPstr5 = strchr(SPACE7, 32),
* MPstr6 = strchr(SPACE8, 32), * MPstr7 = strchr(SPACE9, 32), * MPstr8 = strchr(SPACE10, 32), * MPstr9 = strchr(SPACE11, 32), * MPstr10 = strchr(SPACE12, 32);

//инициализируем указатели на последние пустые клетки вышеуказанного игрового поля для второго игрока
char* SPstr1 = strrchr(SPACE3, 32), * SPstr2 = strrchr(SPACE4, 32), * SPstr3 = strrchr(SPACE5, 32), * SPstr4 = strrchr(SPACE6, 32), * SPstr5 = strrchr(SPACE7, 32),
* SPstr6 = strrchr(SPACE8, 32), * SPstr7 = strrchr(SPACE9, 32), * SPstr8 = strrchr(SPACE10, 32), * SPstr9 = strrchr(SPACE11, 32), * SPstr10 = strrchr(SPACE12, 32);

//функции для обозначения полей, по которым компьютер не будет стрелять, после потопления корабля
//для двухпалубного
void TDSnot(int msv[][SIZZE], int rw1, int cl1, int rw2, int cl2, int horv)
{
    switch (horv)
    {
    case HORIZ:
        if (rw1 == 0 && cl1 == 0)
        {
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw2 == 9 && cl2 == 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw1 == 9 && cl1 == 0)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
        }
        else if (rw2 == 0 && cl2 == 9)
        {
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw1 == 0 && cl1 != 0 && cl2 != 9)
        {
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw1 == 9 && cl1 != 0 && cl2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
        }
        else if (cl1 == 0 && rw1 != 0 && rw2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (cl2 == 9 && rw1 != 0 && rw2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        break;
    case VERT:
        if (rw1 == 0 && cl1 == 0)
        {
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw1 == 0 && cl1 == 9)
        {
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw2 == 9 && cl2 == 0)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 + 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
        }
        else if (rw2 == 9 && cl2 == 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
        }
        else if (cl1 == 0 && rw1 != 0 && rw2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 + 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (cl1 == 9 && rw1 != 0 && rw2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw1 == 0 && cl1 != 0 && cl2 != 9)
        {
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        else if (rw2 == 9 && cl1 != 0 && cl2 != 9)
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 + 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
        }
        else
        {
            msv[rw1 - 1][cl1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 + 1][cl1 - 1] = COMPNOTSHOOT;
            msv[rw1 - 1][cl1 + 1] = COMPNOTSHOOT;
            msv[rw2 - 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 - 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2 + 1] = COMPNOTSHOOT;
            msv[rw2 + 1][cl2] = COMPNOTSHOOT;
        }
        break;
    }

}

//для однопалубного
void ODSnot(int masv[][SIZZE], int r, int c)
{
    if (r == 0 && c == 0)
    {
        masv[r][c + 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c + 1] = COMPNOTSHOOT;
    }
    else if (r == 9 && c == 0)
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c + 1] = COMPNOTSHOOT;
        masv[r][c + 1] = COMPNOTSHOOT;
    }
    else if (r == 0 && c == 9)
    {
        masv[r][c - 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c - 1] = COMPNOTSHOOT;
    }
    else if (r == 9 && c == 9)
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c - 1] = COMPNOTSHOOT;
        masv[r][c - 1] = COMPNOTSHOOT;
    }
    else if (r == 0 && c != 0 && c != 9)
    {
        masv[r][c + 1] = COMPNOTSHOOT;
        masv[r][c - 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c + 1] = COMPNOTSHOOT;
        masv[r + 1][c - 1] = COMPNOTSHOOT;
    }
    else if (r == 9 && c != 0 && c != 9)
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c - 1] = COMPNOTSHOOT;
        masv[r - 1][c + 1] = COMPNOTSHOOT;
        masv[r][c + 1] = COMPNOTSHOOT;
        masv[r][c - 1] = COMPNOTSHOOT;
    }
    else if (c == 0 && r != 0 && r != 9)
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c + 1] = COMPNOTSHOOT;
        masv[r][c + 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c + 1] = COMPNOTSHOOT;
    }
    else if (c == 9 && r != 0 && r != 9)
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c - 1] = COMPNOTSHOOT;
        masv[r][c - 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c - 1] = COMPNOTSHOOT;
    }
    else
    {
        masv[r - 1][c] = COMPNOTSHOOT;
        masv[r - 1][c - 1] = COMPNOTSHOOT;
        masv[r - 1][c + 1] = COMPNOTSHOOT;
        masv[r][c + 1] = COMPNOTSHOOT;
        masv[r][c - 1] = COMPNOTSHOOT;
        masv[r + 1][c] = COMPNOTSHOOT;
        masv[r + 1][c + 1] = COMPNOTSHOOT;
        masv[r + 1][c - 1] = COMPNOTSHOOT;
    }
}

//для трехпалубного
void THDnot(int arg[][SIZZE], int r1, int c1, int r2, int c2, int r3, int c3, int hv)
{
    switch (hv)
    {
    case HORIZ:
        if (r1 == 0 && c1 == 0)
        {
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
        }
        else if (r3 == 9 && c3 == 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
        }
        else if (r1 == 9 && c1 == 0)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
            arg[r3 - 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
        }
        else if (r3 == 0 && c3 == 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 + 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
        }
        else if (r1 == 0 && c1 != 0 && c3 != 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 + 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
        }
        else if (r1 == 9 && c1 != 0 && c3 != 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
            arg[r3 - 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
        }
        else if (c1 == 0 && r1 != 0 && r3 != 9)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
            arg[r3 - 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
        }
        else if (c3 == 9 && r1 != 0 && r3 != 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 + 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
        }
        else
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 + 1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 - 1][c2] = COMPNOTSHOOT;
            arg[r3 - 1][c3] = COMPNOTSHOOT;
            arg[r3 - 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
        }
        break;
    case VERT:
        if (r1 == 0 && c1 == 0)
        {
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
        }
        else if (r3 == 9 && c3 == 9)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
        }
        else if (r3 == 9 && c3 == 0)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 + 1] = COMPNOTSHOOT;
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
        }
        else if (r1 == 0 && c1 == 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
            arg[r3 + 1][c3 - 1] = COMPNOTSHOOT;
        }
        else if (c1 == 0 && r1 != 0 && r3 != 9)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 + 1] = COMPNOTSHOOT;
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
        }
        else if (c1 == 9 && r1 != 0 && r3 != 9)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
            arg[r3 + 1][c3 - 1] = COMPNOTSHOOT;
        }
        else if (r1 == 0 && c1 != 0 && c3 != 9)
        {
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
            arg[r3 + 1][c3 - 1] = COMPNOTSHOOT;
        }
        else if (r3 == 9 && c1 != 0 && c3 != 9)
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 + 1] = COMPNOTSHOOT;
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
        }
        else
        {
            arg[r1 - 1][c1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 - 1] = COMPNOTSHOOT;
            arg[r1 - 1][c1 + 1] = COMPNOTSHOOT;
            arg[r1][c1 - 1] = COMPNOTSHOOT;
            arg[r2][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 + 1] = COMPNOTSHOOT;
            arg[r2 + 1][c2 - 1] = COMPNOTSHOOT;
            arg[r2 + 2][c2 - 1] = COMPNOTSHOOT;
            arg[r3][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3 + 1] = COMPNOTSHOOT;
            arg[r3 + 1][c3] = COMPNOTSHOOT;
            arg[r3 + 1][c3 - 1] = COMPNOTSHOOT;
        }
        break;
    }
}

//для четырехпалубного
void FDnot(int dep[][SIZZE], int d1, int s1, int d2, int s2, int d3, int s3, int d4, int s4, int hz)
{
    switch (hz)
    {
    case HORIZ:
        if (d1 == 0 && s1 == 0)
        {
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        else if (d4 == 9 && s4 == 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
        }
        else if (d1 == 9 && s1 == 0)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
            dep[d4 - 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        else if (d4 == 0 && s4 == 9)
        {
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 + 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
        }
        else if (d1 == 0 && s1 != 0 && s4 != 9)
        {
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 + 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        else if (d1 == 9 && s1 != 0 && s4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
            dep[d4 - 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        else if (s1 == 0 && d1 != 0 && d4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 - 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        else if (s4 == 9 && d1 != 0 && d4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 + 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
        }
        else
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 + 1][s1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 + 1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2] = COMPNOTSHOOT;
            dep[d2 + 1][s2] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 2] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3] = COMPNOTSHOOT;
            dep[d3 - 1][s3] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 - 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4][s4 + 1] = COMPNOTSHOOT;
        }
        break;
    case VERT:
        if (d1 == 0 && s1 == 0)
        {
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
        }
        else if (d4 == 9 && s4 == 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
        }
        else if (d4 == 9 && s4 == 0)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 + 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
        }
        else if (d1 == 0 && s1 == 9)
        {
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
            dep[d4 + 1][s4 - 1] = COMPNOTSHOOT;
        }
        else if (s1 == 0 && d1 != 0 && d4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 + 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
        }
        else if (s1 == 9 && d1 != 0 && d4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
            dep[d4 + 1][s4 - 1] = COMPNOTSHOOT;
        }
        else if (d1 == 0 && s1 != 0 && s4 != 9)
        {
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
            dep[d4 + 1][s4 - 1] = COMPNOTSHOOT;
        }
        else if (d4 == 9 && s1 != 0 && s4 != 9)
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 + 1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
        }
        else
        {
            dep[d1 - 1][s1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 - 1] = COMPNOTSHOOT;
            dep[d1 - 1][s1 + 1] = COMPNOTSHOOT;
            dep[d1][s1 - 1] = COMPNOTSHOOT;
            dep[d2 - 1][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 + 1] = COMPNOTSHOOT;
            dep[d2][s2 - 1] = COMPNOTSHOOT;
            dep[d2 + 1][s2 + 1] = COMPNOTSHOOT;
            dep[d3][s3 - 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 + 1] = COMPNOTSHOOT;
            dep[d3 + 1][s3 - 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4 + 1] = COMPNOTSHOOT;
            dep[d4 + 1][s4] = COMPNOTSHOOT;
            dep[d4 + 1][s4 - 1] = COMPNOTSHOOT;
        }
        break;
    }
}

//функция копирования для массива под отрисовку для возможности смены экранов
void PvC(int play[][SIZZE], int playscr[][SIZZE])
{
    for (int i = 0; i < SIZZE; i++)
    {
        for (int j = 0; j < SIZZE; j++)
        {
            switch (play[i][j])
            {
            case EMPTY:
                playscr[i][j] = EMPTY;
                break;
            case HIT:
                playscr[i][j] = HIT;
                break;
            case KILL:
                playscr[i][j] = KILL;
                break;
            case MISS:
                playscr[i][j] = MISS;
                break;
            case SHIP:
                playscr[i][j] = SHIP;
                break;
            case ODSHIP:
                playscr[i][j] = SHIP;
                break;
            case TDSHIP:
                playscr[i][j] = SHIP;
                break;
            case THDSHIP:
                playscr[i][j] = SHIP;
                break;
            case FDSHIP:
                playscr[i][j] = SHIP;
                break;
            case COMPNOTSHOOT:
                playscr[i][j] = MISS;
                break;
            }
        }
    }
}

//функция отрисовки игрового поля
void DrawingSpace(int scre1[][SIZZE], int scre2[][SIZZE])
{
    for (int i = 0; i < SIZZE; i++)
    {
        switch (scre1[0][i])
        {
        case EMPTY:
            *(MPstr1 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr1 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr1 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr1 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr1 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr1 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[1][i])
        {
        case EMPTY:
            *(MPstr2 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr2 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr2 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr2 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr2 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr2 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[2][i])
        {
        case EMPTY:
            *(MPstr3 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr3 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr3 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr3 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr3 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr3 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[3][i])
        {
        case EMPTY:
            *(MPstr4 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr4 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr4 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr4 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr4 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr4 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[4][i])
        {
        case EMPTY:
            *(MPstr5 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr5 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr5 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr5 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr5 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr5 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[5][i])
        {
        case EMPTY:
            *(MPstr6 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr6 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr6 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr6 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr6 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr6 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[6][i])
        {
        case EMPTY:
            *(MPstr7 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr7 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr7 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr7 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr7 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr7 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[7][i])
        {
        case EMPTY:
            *(MPstr8 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr8 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr8 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr8 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr8 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr8 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[8][i])
        {
        case EMPTY:
            *(MPstr9 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr9 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr9 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr9 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr9 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr9 + i) = wdraw[MISS];
            break;
        }
        switch (scre1[9][i])
        {
        case EMPTY:
            *(MPstr10 + i) = wdraw[EMPTY];
            break;
        case HIT:
            *(MPstr10 + i) = wdraw[HIT];
            break;
        case KILL:
            *(MPstr10 + i) = wdraw[KILL];
            break;
        case MISS:
            *(MPstr10 + i) = wdraw[MISS];
            break;
        case SHIP:
            *(MPstr10 + i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(MPstr10 + i) = wdraw[MISS];
            break;
        }
    }

    int k = 0;
    for (int i = 9; i >= 0; i--)
    {
        switch (scre2[0][k])
        {
        case EMPTY:
            *(SPstr1 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr1 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr1 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr1 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr1 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr1 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[1][k])
        {
        case EMPTY:
            *(SPstr2 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr2 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr2 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr2 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr2 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr2 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[2][k])
        {
        case EMPTY:
            *(SPstr3 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr3 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr3 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr3 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr3 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr3 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[3][k])
        {
        case EMPTY:
            *(SPstr4 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr4 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr4 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr4 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr4 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr4 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[4][k])
        {
        case EMPTY:
            *(SPstr5 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr5 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr5 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr5 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr5 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr5 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[5][k])
        {
        case EMPTY:
            *(SPstr6 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr6 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr6 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr6 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr6 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr6 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[6][k])
        {
        case EMPTY:
            *(SPstr7 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr7 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr7 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr7 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr7 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr7 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[7][k])
        {
        case EMPTY:
            *(SPstr8 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr8 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr8 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr8 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr8 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr8 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[8][k])
        {
        case EMPTY:
            *(SPstr9 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr9 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr9 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr9 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr9 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr9 - i) = wdraw[MISS];
            break;
        }
        switch (scre2[9][k])
        {
        case EMPTY:
            *(SPstr10 - i) = wdraw[EMPTY];
            break;
        case HIT:
            *(SPstr10 - i) = wdraw[HIT];
            break;
        case KILL:
            *(SPstr10 - i) = wdraw[KILL];
            break;
        case MISS:
            *(SPstr10 - i) = wdraw[MISS];
            break;
        case SHIP:
            *(SPstr10 - i) = wdraw[SHIP];
            break;
        case COMPNOTSHOOT:
            *(SPstr10 - i) = wdraw[MISS];
            break;
        }
        k++;
    }

    puts(SPACE1);
    puts(SPACE2);
    puts(SPACE3);
    puts(SPACE4);
    puts(SPACE5);
    puts(SPACE6);
    puts(SPACE7);
    puts(SPACE8);
    puts(SPACE9);
    puts(SPACE10);
    puts(SPACE11);
    puts(SPACE12);
    puts(SPACE13);
}

//функция для ручной расстановки кораблей игрока
void ManualPlaceShip(int ar[][SIZZE], int arscr[][SIZZE], int enscr[][SIZZE])
{
    int ver = 0, gor = 0, counter = 20, digchoice = 11, endd = 1, orient = 2, dir = 5;
    char letchoice = 'z';
    system("CLS");
    PvC(ar, arscr);
    DrawingSpace(arscr, enscr);
    //делаем один четырехпалубный
    do
    {
        cout << "Enter the orientation of the four-deck ship\n0 - horizontal\n1 - vertical\n";
        cin >> orient;
        switch (orient)
        {
        case HORIZ:
            cout << "Enter the direction of the four-deck ship's bow\n0 - right\n1 - left\n";
            cin >> dir;
            break;
        case VERT:
            cout << "Enter the direction of the four-deck ship's bow\n2 - up\n3 - down\n";
            cin >> dir;
            break;
        }
        cout << "Enter the letter of main coordinate of four-deck ship in the format A-J \n";
        cin >> letchoice;
        cout << "Enter the digit of main coordinate of four-deck ship in the format 1 - 10\n";
        cin >> digchoice;
        if (((letchoice >= (char(65)) && letchoice <= (char(74))) || ((letchoice >= (char(97)) && letchoice <= (char(106))))) && (digchoice >= 1 && digchoice <= 10) && (orient >= 0 && orient < 2) && (dir >= 0 && dir < 4))
        {
            switch (orient)
            {
            case HORIZ:
                if ((dir == RIGHT && letchoice > char(71) && letchoice <= (char(74))) || (dir == RIGHT && letchoice > char(103))) dir = LEFT;
                else if ((dir == LEFT && letchoice < char(68)) || (dir == LEFT && letchoice < char(100))) dir = RIGHT;
                break;
            case VERT:
                if (digchoice < 4 && dir == UP) dir = DOWN;
                else if (digchoice > 7 && dir == DOWN) dir = UP;
                break;
            }
            endd = 0;
        }
    } while (endd);
    endd = 1;
    switch (letchoice)
    {
    case 'A':
    case 'a':
        ver = 0;
        break;
    case 'B':
    case 'b':
        ver = 1;
        break;
    case 'C':
    case 'c':
        ver = 2;
        break;
    case 'D':
    case 'd':
        ver = 3;
        break;
    case 'E':
    case 'e':
        ver = 4;
        break;
    case 'F':
    case 'f':
        ver = 5;
        break;
    case 'G':
    case 'g':
        ver = 6;
        break;
    case 'H':
    case 'h':
        ver = 7;
        break;
    case 'I':
    case 'i':
        ver = 8;
        break;
    case 'J':
    case 'j':
        ver = 9;
        break;
    }
    switch (digchoice)
    {
    case 1:
        gor = 0;
        break;
    case 2:
        gor = 1;
        break;
    case 3:
        gor = 2;
        break;
    case 4:
        gor = 3;
        break;
    case 5:
        gor = 4;
        break;
    case 6:
        gor = 5;
        break;
    case 7:
        gor = 6;
        break;
    case 8:
        gor = 7;
        break;
    case 9:
        gor = 8;
        break;
    case 10:
        gor = 9;
        break;
    }
    switch (dir)
    {
    case RIGHT:
        ar[gor][ver] = FDSHIP;
        ar[gor][ver + 1] = FDSHIP;
        ar[gor][ver + 2] = FDSHIP;
        ar[gor][ver + 3] = FDSHIP;
        break;
    case LEFT:
        ar[gor][ver] = FDSHIP;
        ar[gor][ver - 1] = FDSHIP;
        ar[gor][ver - 2] = FDSHIP;
        ar[gor][ver - 3] = FDSHIP;
        break;
    case UP:
        ar[gor][ver] = FDSHIP;
        ar[gor - 1][ver] = FDSHIP;
        ar[gor - 2][ver] = FDSHIP;
        ar[gor - 3][ver] = FDSHIP;
        break;
    case DOWN:
        ar[gor][ver] = FDSHIP;
        ar[gor + 1][ver] = FDSHIP;
        ar[gor + 2][ver] = FDSHIP;
        ar[gor + 3][ver] = FDSHIP;
        break;
    }
    system("CLS");
    PvC(ar, arscr);
    DrawingSpace(arscr, enscr);
    //делаем два трехпалубных
    for (int j = 0; j < 2; j++)
    {
        do
        {
            cout << "Enter the orientation of " << j + 1 << " three-deck ship\n0 - horizontal\n1 - vertical\n";
            cin >> orient;
            switch (orient)
            {
            case HORIZ:
                cout << "Enter the direction of " << j + 1 << " three-deck ship's bow\n0 - right\n1 - left\n";
                cin >> dir;
                break;
            case VERT:
                cout << "Enter the direction of " << j + 1 << " three-deck ship's bow\n2 - up\n3 - down\n";
                cin >> dir;
                break;
            }
            cout << "Enter the letter of main coordinate of " << j + 1 << " three-deck ship in the format A-J \n";
            cin >> letchoice;
            cout << "Enter the digit of main coordinate of " << j + 1 << " three-deck ship in the format 1 - 10\n";
            cin >> digchoice;
            if (((letchoice >= (char(65)) && letchoice <= (char(74))) || ((letchoice >= (char(97)) && letchoice <= (char(106))))) && (digchoice >= 1 && digchoice <= 10) && (orient >= 0 && orient < 2) && (dir >= 0 && dir < 4))
            {
                switch (orient)
                {
                case HORIZ:
                    if ((dir == RIGHT && letchoice > char(72)) || (dir == RIGHT && letchoice > char(104))) dir = LEFT;
                    else if ((dir == LEFT && letchoice < char(67)) || (dir == LEFT && letchoice < char(99))) dir = RIGHT;
                    break;
                case VERT:
                    if (digchoice < 3 && dir == UP) dir = DOWN;
                    else if (digchoice > 8 && dir == DOWN) dir = UP;
                    break;
                }
                endd = 0;
            }
        } while (endd);
        endd = 1;
        switch (letchoice)
        {
        case 'A':
        case 'a':
            ver = 0;
            break;
        case 'B':
        case 'b':
            ver = 1;
            break;
        case 'C':
        case 'c':
            ver = 2;
            break;
        case 'D':
        case 'd':
            ver = 3;
            break;
        case 'E':
        case 'e':
            ver = 4;
            break;
        case 'F':
        case 'f':
            ver = 5;
            break;
        case 'G':
        case 'g':
            ver = 6;
            break;
        case 'H':
        case 'h':
            ver = 7;
            break;
        case 'I':
        case 'i':
            ver = 8;
            break;
        case 'J':
        case 'j':
            ver = 9;
            break;
        }
        switch (digchoice)
        {
        case 1:
            gor = 0;
            break;
        case 2:
            gor = 1;
            break;
        case 3:
            gor = 2;
            break;
        case 4:
            gor = 3;
            break;
        case 5:
            gor = 4;
            break;
        case 6:
            gor = 5;
            break;
        case 7:
            gor = 6;
            break;
        case 8:
            gor = 7;
            break;
        case 9:
            gor = 8;
            break;
        case 10:
            gor = 9;
            break;
        }
        switch (dir)
        {
        case RIGHT:
            ar[gor][ver] = THDSHIP;
            ar[gor][ver + 1] = THDSHIP;
            ar[gor][ver + 2] = THDSHIP;
            break;
        case LEFT:
            ar[gor][ver] = THDSHIP;
            ar[gor][ver - 1] = THDSHIP;
            ar[gor][ver - 2] = THDSHIP;
            break;
        case UP:
            ar[gor][ver] = THDSHIP;
            ar[gor - 1][ver] = THDSHIP;
            ar[gor - 2][ver] = THDSHIP;
            break;
        case DOWN:
            ar[gor][ver] = THDSHIP;
            ar[gor + 1][ver] = THDSHIP;
            ar[gor + 2][ver] = THDSHIP;
            break;
        }
        system("CLS");
        PvC(ar, arscr);
        DrawingSpace(arscr, enscr);
    }
    //вводим двухпалубные корабли
    for (int j = 0; j < 3; j++)
    {
        do
        {
            cout << "Enter the orientation of " << j + 1 << " two-deck ship\n0 - horizontal\n1 - vertical\n";
            cin >> orient;
            switch (orient)
            {
            case HORIZ:
                cout << "Enter the direction of " << j + 1 << " two-deck ship's bow\n0 - right\n1 - left\n";
                cin >> dir;
                break;
            case VERT:
                cout << "Enter the direction of " << j + 1 << " two-deck ship's bow\n2 - up\n3 - down\n";
                cin >> dir;
                break;
            }
            cout << "Enter the letter of main coordinate of " << j + 1 << " two-deck ship in the format A-J \n";
            cin >> letchoice;
            cout << "Enter the digit of main coordinate of " << j + 1 << " two-deck ship in the format 1 - 10\n";
            cin >> digchoice;
            if (((letchoice >= (char(65)) && letchoice <= (char(74))) || ((letchoice >= (char(97)) && letchoice <= (char(106))))) && (digchoice >= 1 && digchoice <= 10) && (orient >= 0 && orient < 2) && (dir >= 0 && dir < 4))
            {
                switch (orient)
                {
                case HORIZ:
                    if ((dir == RIGHT && letchoice == char(74)) || (dir == RIGHT && letchoice == char(106))) dir = LEFT;
                    else if ((dir == LEFT && letchoice == char(65)) || (dir == LEFT && letchoice == char(97))) dir = RIGHT;
                    break;
                case VERT:
                    if (digchoice == 1 && dir == UP) dir = DOWN;
                    else if (digchoice == 10 && dir == DOWN) dir = UP;
                    break;
                }
                endd = 0;
            }
        } while (endd);
        endd = 1;
        switch (letchoice)
        {
        case 'A':
        case 'a':
            ver = 0;
            break;
        case 'B':
        case 'b':
            ver = 1;
            break;
        case 'C':
        case 'c':
            ver = 2;
            break;
        case 'D':
        case 'd':
            ver = 3;
            break;
        case 'E':
        case 'e':
            ver = 4;
            break;
        case 'F':
        case 'f':
            ver = 5;
            break;
        case 'G':
        case 'g':
            ver = 6;
            break;
        case 'H':
        case 'h':
            ver = 7;
            break;
        case 'I':
        case 'i':
            ver = 8;
            break;
        case 'J':
        case 'j':
            ver = 9;
            break;
        }
        switch (digchoice)
        {
        case 1:
            gor = 0;
            break;
        case 2:
            gor = 1;
            break;
        case 3:
            gor = 2;
            break;
        case 4:
            gor = 3;
            break;
        case 5:
            gor = 4;
            break;
        case 6:
            gor = 5;
            break;
        case 7:
            gor = 6;
            break;
        case 8:
            gor = 7;
            break;
        case 9:
            gor = 8;
            break;
        case 10:
            gor = 9;
            break;
        }
        switch (dir)
        {
        case RIGHT:
            ar[gor][ver] = TDSHIP;
            ar[gor][ver + 1] = TDSHIP;
            break;
        case LEFT:
            ar[gor][ver] = TDSHIP;
            ar[gor][ver - 1] = TDSHIP;
            break;
        case UP:
            ar[gor][ver] = TDSHIP;
            ar[gor - 1][ver] = TDSHIP;
            break;
        case DOWN:
            ar[gor][ver] = TDSHIP;
            ar[gor + 1][ver] = TDSHIP;
            break;
        }
        system("CLS");
        PvC(ar, arscr);
        DrawingSpace(arscr, enscr);
    }
    //вводим однопалубные корабли
    for (int j = 0; j < 4; j++)
    {
        do
        {
            cout << "Enter the letter of coordinate of " << j + 1 << " one-deck ship in the format A-J \n";
            cin >> letchoice;
            cout << "Enter the digit of coordinate of " << j + 1 << " one-deck ship in the format 1 - 10\n";
            cin >> digchoice;
            if (((letchoice >= (char(65)) && letchoice <= (char(74))) || ((letchoice >= (char(97)) && letchoice <= (char(106))))) && (digchoice >= 1 && digchoice <= 10)) endd = 0;
        } while (endd);
        endd = 1;
        switch (letchoice)
        {
        case 'A':
        case 'a':
            ver = 0;
            break;
        case 'B':
        case 'b':
            ver = 1;
            break;
        case 'C':
        case 'c':
            ver = 2;
            break;
        case 'D':
        case 'd':
            ver = 3;
            break;
        case 'E':
        case 'e':
            ver = 4;
            break;
        case 'F':
        case 'f':
            ver = 5;
            break;
        case 'G':
        case 'g':
            ver = 6;
            break;
        case 'H':
        case 'h':
            ver = 7;
            break;
        case 'I':
        case 'i':
            ver = 8;
            break;
        case 'J':
        case 'j':
            ver = 9;
            break;
        }
        switch (digchoice)
        {
        case 1:
            gor = 0;
            break;
        case 2:
            gor = 1;
            break;
        case 3:
            gor = 2;
            break;
        case 4:
            gor = 3;
            break;
        case 5:
            gor = 4;
            break;
        case 6:
            gor = 5;
            break;
        case 7:
            gor = 6;
            break;
        case 8:
            gor = 7;
            break;
        case 9:
            gor = 8;
            break;
        case 10:
            gor = 9;
            break;
        }
        ar[gor][ver] = ODSHIP;
        system("CLS");
        PvC(ar, arscr);
        DrawingSpace(arscr, enscr);
    }
}

//функция для расстановки по одному кораблю
void PutShip(int pole[][SIZZE], int pal)
{
    int orien = 0, x = 0, y = 0, end = 1;
    switch (pal)
    {
    case FDSHIP:
        while (end)
        {
            orien = rand() % 2;
            switch (orien)
            {
            case HORIZ:
                x = rand() % 10;
                y = rand() % 10;
                if (y <= 6)
                {
                    pole[x][y] = FDSHIP;
                    pole[x][y + 1] = FDSHIP;
                    pole[x][y + 2] = FDSHIP;
                    pole[x][y + 3] = FDSHIP;
                    FDnot(pole, x, y, x, y + 1, x, y + 2, x, y + 3, HORIZ);
                    end = 0;
                }
                else
                {
                    pole[x][y] = FDSHIP;
                    pole[x][y - 1] = FDSHIP;
                    pole[x][y - 2] = FDSHIP;
                    pole[x][y - 3] = FDSHIP;
                    FDnot(pole, x, y - 3, x, y - 2, x, y - 1, x, y, HORIZ);
                    end = 0;
                }
                break;
            case VERT:
                x = rand() % 10;
                y = rand() % 10;
                if (x <= 6)
                {
                    pole[x][y] = FDSHIP;
                    pole[x + 1][y] = FDSHIP;
                    pole[x + 2][y] = FDSHIP;
                    pole[x + 3][y] = FDSHIP;
                    FDnot(pole, x, y, x + 1, y, x + 2, y, x + 3, y, VERT);
                    end = 0;
                }
                else
                {
                    pole[x][y] = FDSHIP;
                    pole[x - 1][y] = FDSHIP;
                    pole[x - 2][y] = FDSHIP;
                    pole[x - 3][y] = FDSHIP;
                    FDnot(pole, x - 3, y, x - 2, y, x - 1, y, x, y, VERT);
                    end = 0;
                }
                break;
            }
        }
        break;
    case THDSHIP:
        orien = rand() % 2;
        switch (orien)
        {
        case HORIZ:
            while (end)
            {
                x = rand() % 10;
                y = rand() % 10;
                if (y <= 7)
                {
                    if (pole[x][y] != FDSHIP && pole[x][y + 1] != FDSHIP && pole[x][y + 2] != FDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x][y + 1] != COMPNOTSHOOT && pole[x][y + 2] != COMPNOTSHOOT)
                    {
                        pole[x][y] = THDSHIP; pole[x][y + 1] = THDSHIP; pole[x][y + 2] = THDSHIP;
                        THDnot(pole, x, y, x, y + 1, x, y + 2, HORIZ);
                        end = 0;
                    }
                }
                else
                {
                    if (pole[x][y] != FDSHIP && pole[x][y - 1] != FDSHIP && pole[x][y - 2] != FDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x][y - 1] != COMPNOTSHOOT && pole[x][y - 2] != COMPNOTSHOOT)
                    {
                        pole[x][y] = THDSHIP; pole[x][y - 1] = THDSHIP; pole[x][y - 2] = THDSHIP;
                        THDnot(pole, x, y - 2, x, y - 1, x, y, HORIZ);
                        end = 0;
                    }
                }
            }
            break;
        case VERT:
            while (end)
            {
                x = rand() % 10;
                y = rand() % 10;
                if (x <= 7)
                {
                    if (pole[x][y] != FDSHIP && pole[x + 1][y] != FDSHIP && pole[x + 2][y] != FDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x + 1][y] != COMPNOTSHOOT && pole[x + 2][y] != COMPNOTSHOOT)
                    {
                        pole[x][y] = THDSHIP; pole[x + 1][y] = THDSHIP; pole[x + 2][y] = THDSHIP;
                        THDnot(pole, x, y, x + 1, y, x + 2, y, VERT);
                        end = 0;
                    }
                }
                else
                {
                    if (pole[x][y] != FDSHIP && pole[x - 1][y] != FDSHIP && pole[x - 2][y] != FDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x - 1][y] != COMPNOTSHOOT && pole[x - 2][y] != COMPNOTSHOOT)
                    {
                        pole[x][y] = THDSHIP; pole[x - 1][y] = THDSHIP; pole[x - 2][y] = THDSHIP;
                        THDnot(pole, x - 2, y, x - 1, y, x, y, VERT);
                        end = 0;
                    }
                }
            }
            break;
        }
        break;
    case TDSHIP:
        orien = rand() % 2;
        switch (orien)
        {
        case HORIZ:
            while (end)
            {
                x = rand() % 10;
                y = rand() % 10;
                if (y <= 8)
                {
                    if (pole[x][y] != FDSHIP && pole[x][y + 1] != FDSHIP && pole[x][y] != THDSHIP && pole[x][y + 1] != THDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x][y + 1] != COMPNOTSHOOT)
                    {
                        pole[x][y] = TDSHIP; pole[x][y + 1] = TDSHIP;
                        TDSnot(pole, x, y, x, y + 1, HORIZ);
                        end = 0;
                    }
                }
                else
                {
                    if (pole[x][y] != FDSHIP && pole[x][y - 1] != FDSHIP && pole[x][y] != THDSHIP && pole[x][y - 1] != THDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x][y - 1] != COMPNOTSHOOT)
                    {
                        pole[x][y] = TDSHIP; pole[x][y - 1] = TDSHIP;
                        TDSnot(pole, x, y - 1, x, y, HORIZ);
                        end = 0;
                    }
                }
            }
            break;
        case VERT:
            while (end)
            {
                x = rand() % 10;
                y = rand() % 10;
                if (x <= 8)
                {
                    if (pole[x][y] != FDSHIP && pole[x + 1][y] != FDSHIP && pole[x][y] != THDSHIP && pole[x + 1][y] != THDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x + 1][y] != COMPNOTSHOOT)
                    {
                        pole[x][y] = TDSHIP; pole[x + 1][y] = TDSHIP;
                        TDSnot(pole, x, y, x + 1, y, VERT);
                        end = 0;
                    }
                }
                else
                {
                    if (pole[x][y] != FDSHIP && pole[x - 1][y] != FDSHIP && pole[x][y] != THDSHIP && pole[x - 1][y] != THDSHIP && pole[x][y] != COMPNOTSHOOT && pole[x - 1][y] != COMPNOTSHOOT)
                    {
                        pole[x][y] = TDSHIP; pole[x - 1][y] = TDSHIP;
                        TDSnot(pole, x - 1, y, x, y, VERT);
                        end = 0;
                    }
                }
            }
            break;
        }
        break;
    case ODSHIP:
        while (end)
        {
            x = rand() % 10;
            y = rand() % 10;
            if (pole[x][y] != FDSHIP && pole[x][y] != THDSHIP && pole[x][y] != TDSHIP && pole[x][y] != COMPNOTSHOOT)
            {
                pole[x][y] = ODSHIP;
                ODSnot(pole, x, y);
                end = 0;
            }
        }
        break;
    }
}

//функция для автоматической расстановки корбалей ВАРИАНТ ВТОРОЙ!!!!!!
void AutoGenShip(int mas[][SIZZE])
{
    PutShip(mas, FDSHIP);

    for (int z = 0; z < 2; z++)
        PutShip(mas, THDSHIP);

    for (int l = 0; l < 3; l++)
        PutShip(mas, TDSHIP);

    for (int p = 0; p < 4; p++)
        PutShip(mas, ODSHIP);

    for (int i = 0; i < SIZZE; i++)
    {
        for (int j = 0; j < SIZZE; j++)
        {
            if (mas[i][j] == COMPNOTSHOOT) mas[i][j] = EMPTY;
        }
    }
}

//функция управляемого человеком выстрела по цели
int PlayerShoot(int target[][SIZZE], int tarinfo[][SIZZE], int shooter[][SIZZE], int firstscr[][SIZZE])
{
    int dig = 0, gr = 0, vr = 0, ndd = 1;
    char let = '0';
    do
    {
        cout << "\nEnter the letter of coordinates of the shoot in the format A - J\n";
        cin >> let;
        cout << "\nEnter the digit of coordinates of the shoot in the format 1 - 10\n";
        cin >> dig;
        if (((let >= (char(65)) && let <= (char(74))) || ((let >= (char(97)) && let <= (char(106))))) && (dig >= 1 && dig <= 10)) ndd = 0;
    } while (ndd);
    switch (let)
    {
    case 'A':
    case 'a':
        vr = 0;
        break;
    case 'B':
    case 'b':
        vr = 1;
        break;
    case 'C':
    case 'c':
        vr = 2;
        break;
    case 'D':
    case 'd':
        vr = 3;
        break;
    case 'E':
    case 'e':
        vr = 4;
        break;
    case 'F':
    case 'f':
        vr = 5;
        break;
    case 'G':
    case 'g':
        vr = 6;
        break;
    case 'H':
    case 'h':
        vr = 7;
        break;
    case 'I':
    case 'i':
        vr = 8;
        break;
    case 'J':
    case 'j':
        vr = 9;
        break;
    }
    switch (dig)
    {
    case 1:
        gr = 0;
        break;
    case 2:
        gr = 1;
        break;
    case 3:
        gr = 2;
        break;
    case 4:
        gr = 3;
        break;
    case 5:
        gr = 4;
        break;
    case 6:
        gr = 5;
        break;
    case 7:
        gr = 6;
        break;
    case 8:
        gr = 7;
        break;
    case 9:
        gr = 8;
        break;
    case 10:
        gr = 9;
        break;
    }
    target[gr][vr] = tarinfo[gr][vr];
    switch (target[gr][vr])
    {
    case EMPTY:
        target[gr][vr] = MISS;
        tarinfo[gr][vr] = MISS;
        system("CLS");
        PvC(shooter, firstscr);
        DrawingSpace(firstscr, target);
        cout << "\nYou missed\n";
        return 0;
        break;
    case HIT:
        cout << "\nYou missed\n";
        return 0;
        break;
    case KILL:
        cout << "\nYou missed\n";
        return 0;
        break;
    case MISS:
        cout << "\nYou missed\n";
        return 0;
        break;
    case ODSHIP:
        target[gr][vr] = KILL;
        tarinfo[gr][vr] = KILL;
        ODSnot(target, gr, vr);
        system("CLS");
        PvC(shooter, firstscr);
        DrawingSpace(firstscr, target);
        cout << "\nThe ship is killed\n";
        return 1;
        break;
    case TDSHIP:
        if (target[gr][vr + 1] == HIT) { TDSnot(target, gr, vr, gr, vr + 1, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr + 1] = KILL; target[gr][vr + 1] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr - 1] == HIT) { TDSnot(target, gr, vr - 1, gr, vr, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr - 1] = KILL; target[gr][vr - 1] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr + 1][vr] == HIT) { TDSnot(target, gr, vr, gr + 1, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; target[gr + 1][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr - 1][vr] == HIT) { TDSnot(target, gr - 1, vr, gr, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr - 1][vr] = KILL; target[gr - 1][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else { tarinfo[gr][vr] = HIT; target[gr][vr] = HIT; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nYou hit the target. Shoot again.\n"; }
        return 1;
        break;
    case THDSHIP:
        if (target[gr][vr + 1] == HIT && target[gr][vr + 2] == HIT) { THDnot(target, gr, vr, gr, vr + 1, gr, vr + 2, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr + 1] = KILL; tarinfo[gr][vr + 2] = KILL; target[gr][vr + 1] = KILL; target[gr][vr + 2] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr + 1] == HIT && target[gr][vr - 1] == HIT) { THDnot(target, gr, vr - 1, gr, vr, gr, vr + 1, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr + 1] = KILL; tarinfo[gr][vr - 1] = KILL; target[gr][vr + 1] = KILL; target[gr][vr - 1] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr - 1] == HIT && target[gr][vr - 2] == HIT) { THDnot(target, gr, vr - 2, gr, vr - 1, gr, vr, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr - 1] = KILL; tarinfo[gr][vr - 2] = KILL; target[gr][vr - 1] = KILL; target[gr][vr - 2] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr + 1][vr] == HIT && target[gr + 2][vr] == HIT) { THDnot(target, gr, vr, gr + 1, vr, gr + 2, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; tarinfo[gr + 2][vr] = KILL; target[gr + 1][vr] = KILL; target[gr + 2][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr + 1][vr] == HIT && target[gr - 1][vr] == HIT) { THDnot(target, gr - 1, vr, gr, vr, gr + 1, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; tarinfo[gr - 1][vr] = KILL; target[gr + 1][vr] = KILL; target[gr - 1][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr - 1][vr] == HIT && target[gr - 2][vr] == HIT) { THDnot(target, gr - 2, vr, gr - 1, vr, gr, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr - 1][vr] = KILL; tarinfo[gr - 2][vr] = KILL; target[gr - 1][vr] = KILL; target[gr - 2][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else { tarinfo[gr][vr] = HIT; target[gr][vr] = HIT; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nYou hit the target. Shoot again.\n"; }
        return 1;
        break;
    case FDSHIP:
        if (target[gr + 1][vr] == HIT && target[gr + 2][vr] == HIT && target[gr + 3][vr] == HIT) { FDnot(target, gr, vr, gr + 1, vr, gr + 2, vr, gr + 3, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; tarinfo[gr + 2][vr] = KILL; tarinfo[gr + 3][vr] = KILL; target[gr + 1][vr] = KILL; target[gr + 2][vr] = KILL; target[gr + 3][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr - 1][vr] == HIT && target[gr - 2][vr] == HIT && target[gr - 3][vr] == HIT) { FDnot(target, gr - 3, vr, gr - 2, vr, gr - 1, vr, gr, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr - 1][vr] = KILL; tarinfo[gr - 2][vr] = KILL; tarinfo[gr - 3][vr] = KILL; target[gr - 1][vr] = KILL; target[gr - 2][vr] = KILL; target[gr - 3][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr - 1][vr] == HIT && target[gr + 1][vr] == HIT && target[gr + 2][vr] == HIT) { FDnot(target, gr - 1, vr, gr, vr, gr + 1, vr, gr + 2, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; tarinfo[gr + 2][vr] = KILL; tarinfo[gr - 1][vr] = KILL; target[gr + 1][vr] = KILL; target[gr + 2][vr] = KILL; target[gr - 1][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr + 1][vr] == HIT && target[gr - 1][vr] == HIT && target[gr - 2][vr] == HIT) { FDnot(target, gr - 2, vr, gr - 1, vr, gr, vr, gr + 1, vr, VERT); tarinfo[gr][vr] = KILL; tarinfo[gr + 1][vr] = KILL; tarinfo[gr - 1][vr] = KILL; tarinfo[gr - 2][vr] = KILL; target[gr + 1][vr] = KILL; target[gr - 2][vr] = KILL; target[gr - 1][vr] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr + 1] == HIT && target[gr][vr + 2] == HIT && target[gr][vr + 3] == HIT) { FDnot(target, gr, vr, gr, vr + 1, gr, vr + 2, gr, vr + 3, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr + 1] = KILL; tarinfo[gr][vr + 2] = KILL; tarinfo[gr][vr + 3] = KILL; target[gr][vr + 1] = KILL; target[gr][vr + 2] = KILL; target[gr][vr + 3] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr - 1] == HIT && target[gr][vr - 2] == HIT && target[gr][vr - 3] == HIT) { FDnot(target, gr, vr - 3, gr, vr - 2, gr, vr - 1, gr, vr, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr - 1] = KILL; tarinfo[gr][vr - 2] = KILL; tarinfo[gr][vr - 3] = KILL; target[gr][vr - 1] = KILL; target[gr][vr - 2] = KILL; target[gr][vr - 3] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr - 1] == HIT && target[gr][vr + 1] == HIT && target[gr][vr + 2] == HIT) { FDnot(target, gr, vr - 1, gr, vr, gr, vr + 1, gr, vr + 2, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr - 1] = KILL; tarinfo[gr][vr + 1] = KILL; tarinfo[gr][vr + 2] = KILL; target[gr][vr - 1] = KILL; target[gr][vr + 1] = KILL; target[gr][vr + 2] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else if (target[gr][vr + 1] == HIT && target[gr][vr - 1] == HIT && target[gr][vr - 2] == HIT) { FDnot(target, gr, vr - 2, gr, vr - 1, gr, vr, gr, vr + 1, HORIZ); tarinfo[gr][vr] = KILL; tarinfo[gr][vr + 1] = KILL; tarinfo[gr][vr - 1] = KILL; tarinfo[gr][vr - 2] = KILL; target[gr][vr + 1] = KILL; target[gr][vr - 1] = KILL; target[gr][vr - 2] = KILL; target[gr][vr] = KILL; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nThe ship is killed\n"; }
        else { tarinfo[gr][vr] = HIT; target[gr][vr] = HIT; system("CLS"); PvC(shooter, firstscr); DrawingSpace(firstscr, target); cout << "\nYou hit the target. Shoot again.\n"; }
        return 1;
        break;
    }
}

//функция выстрела компьютера по указанной координате
int CShoot(int kuda[][SIZZE], int sr, int sl, int per[][SIZZE], int vtor[][SIZZE])
{
    switch (kuda[sr][sl])
    {
    case EMPTY:
        kuda[sr][sl] = MISS;
        system("CLS");
        PvC(kuda, per);
        DrawingSpace(per, vtor);
        cout << "\nThe computer missed\n";
        return 0;
        break;
    case ODSHIP:
        kuda[sr][sl] = KILL;
        ODSnot(kuda, sr, sl);
        system("CLS");
        PvC(kuda, per);
        DrawingSpace(per, vtor);
        cout << "\nYour ship is killed\n";
        return KILL;
        break;
    case TDSHIP:
        if (kuda[sr][sl + 1] == HIT) { kuda[sr][sl + 1] = KILL;  kuda[sr][sl] = KILL; TDSnot(kuda, sr, sl, sr, sl + 1, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl - 1] == HIT) { kuda[sr][sl - 1] = KILL; kuda[sr][sl] = KILL; TDSnot(kuda, sr, sl - 1, sr, sl, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr + 1][sl] == HIT) { kuda[sr + 1][sl] = KILL; kuda[sr][sl] = KILL; TDSnot(kuda, sr, sl, sr + 1, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr - 1][sl] == HIT) { kuda[sr - 1][sl] = KILL; kuda[sr][sl] = KILL; TDSnot(kuda, sr - 1, sl, sr, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else { kuda[sr][sl] = HIT; system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nComputer hit the target and shooting again...\n"; }
        return HIT;
        break;
    case THDSHIP:
        if (kuda[sr][sl + 1] == HIT && kuda[sr][sl + 2] == HIT) { kuda[sr][sl + 1] = KILL; kuda[sr][sl + 2] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr, sl, sr, sl + 1, sr, sl + 2, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl + 1] == HIT && kuda[sr][sl - 1] == HIT) { kuda[sr][sl + 1] = KILL; kuda[sr][sl - 1] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr, sl - 1, sr, sl, sr, sl + 1, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl - 1] == HIT && kuda[sr][sl - 2] == HIT) { kuda[sr][sl - 1] = KILL; kuda[sr][sl - 2] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr, sl - 2, sr, sl - 1, sr, sl, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr + 1][sl] == HIT && kuda[sr + 2][sl] == HIT) { kuda[sr + 1][sl] = KILL; kuda[sr + 2][sl] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr, sl, sr + 1, sl, sr + 2, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr + 1][sl] == HIT && kuda[sr - 1][sl] == HIT) { kuda[sr + 1][sl] = KILL; kuda[sr - 1][sl] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr - 1, sl, sr, sl, sr + 1, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr - 1][sl] == HIT && kuda[sr - 2][sl] == HIT) { kuda[sr - 1][sl] = KILL; kuda[sr - 2][sl] = KILL; kuda[sr][sl] = KILL; THDnot(kuda, sr - 2, sl, sr - 1, sl, sr, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else { kuda[sr][sl] = HIT; system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nComputer hit the target and shooting again...\n"; }
        return HIT;
        break;
    case FDSHIP:
        if (kuda[sr + 1][sl] == HIT && kuda[sr + 2][sl] == HIT && kuda[sr + 3][sl] == HIT) { kuda[sr + 1][sl] = KILL; kuda[sr + 2][sl] = KILL; kuda[sr + 3][sl] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr, sl, sr + 1, sl, sr + 2, sl, sr + 3, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr - 1][sl] == HIT && kuda[sr - 2][sl] == HIT && kuda[sr - 3][sl] == HIT) { kuda[sr - 1][sl] = KILL; kuda[sr - 2][sl] = KILL; kuda[sr - 3][sl] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr - 3, sl, sr - 2, sl, sr - 1, sl, sr, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr - 1][sl] == HIT && kuda[sr + 1][sl] == HIT && kuda[sr + 2][sl] == HIT) { kuda[sr - 1][sl] = KILL; kuda[sr + 2][sl] = KILL; kuda[sr + 1][sl] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr - 1, sl, sr, sl, sr + 1, sl, sr + 2, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr + 1][sl] == HIT && kuda[sr - 1][sl] == HIT && kuda[sr - 2][sl] == HIT) { kuda[sr + 1][sl] = KILL; kuda[sr - 2][sl] = KILL; kuda[sr - 1][sl] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr - 2, sl, sr - 1, sl, sr, sl, sr + 1, sl, VERT); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl + 1] == HIT && kuda[sr][sl + 2] == HIT && kuda[sr][sl + 3] == HIT) { kuda[sr][sl + 1] = KILL; kuda[sr][sl + 2] = KILL; kuda[sr][sl + 3] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr, sl, sr, sl + 1, sr, sl + 2, sr, sl + 3, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl - 1] == HIT && kuda[sr][sl - 2] == HIT && kuda[sr][sl - 3] == HIT) { kuda[sr][sl - 1] = KILL; kuda[sr][sl - 2] = KILL; kuda[sr][sl - 3] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr, sl - 3, sr, sl - 2, sr, sl - 1, sr, sl, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl - 1] == HIT && kuda[sr][sl + 1] == HIT && kuda[sr][sl + 2] == HIT) { kuda[sr][sl + 1] = KILL; kuda[sr][sl + 2] = KILL; kuda[sr][sl - 1] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr, sl - 1, sr, sl, sr, sl + 1, sr, sl + 2, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else if (kuda[sr][sl + 1] == HIT && kuda[sr][sl - 1] == HIT && kuda[sr][sl - 2] == HIT) { kuda[sr][sl + 1] = KILL; kuda[sr][sl - 2] = KILL; kuda[sr][sl - 1] = KILL; kuda[sr][sl] = KILL; FDnot(kuda, sr, sl - 2, sr, sl - 1, sr, sl, sr, sl + 1, HORIZ); system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nYour ship is killed\n"; return KILL; }
        else { kuda[sr][sl] = HIT; system("CLS"); PvC(kuda, per); DrawingSpace(per, vtor); cout << "\nComputer hit the target and shooting again...\n"; }
        return HIT;
        break;
    }
}

//функция рандомного выстрела компьютера
int CompShoot(int targ[][SIZZE], int fscr[][SIZZE], int sscr[][SIZZE])
{
    int str = 0, stl = 0, endw = 1;
    do
    {
        str = rand() % 10;
        stl = rand() % 10;
        if (targ[str][stl] != HIT && targ[str][stl] != MISS && targ[str][stl] != KILL && targ[str][stl] != COMPNOTSHOOT) endw = 0;
    } while (endw);
    return CShoot(targ, str, stl, fscr, sscr);
}

/*функция для поиска направления для дальнейшего добивания. Описание работы функции:
после попадания при очереди стрельбы компьютера, ИИ переходит в режим определения направления. Внутри фуцнкции запускается двойной цикл
для поиска раненой клетки. Далее случайно выбирается направление для проверки, исключая возможность стрельбы по несуществующим ячейкам массива и по тем ячейкам, в которых уже есть MISS, KILL и COMPNOTSHOOT.
При ранении корабля функцией возвращается направление дальнейшей стрельбы, с условием, что направление вернется противоположным, если проверяемая ячейка будет последней существующей в массиве (равна 9 или 0).
Соответственно дальше, с помощью другой функции комп будет добивать корабль
Если при проверке корабль убит, значит возвращается DONE, что означает возвращение ИИ к рандомной стрельбе
Если при проверке выстрел был неудачным, то возвращается NDONE и как только вновь наступит очередь компьютера он дальше будет проверять область вокруг раненой клетки*/
int FindingDirection(int cel[][SIZZE], int fekr[][SIZZE], int sekr[][SIZZE])
{
    int y = 0, x = 0, zav = 1, naprav;
    //находим раненную палубу
    for (int i = 0; i < SIZZE; i++)
    {
        for (int j = 0; j < SIZZE; j++)
        {
            if (cel[i][j] == HIT)
            {
                y = i;
                x = j;
            }
        }
    }
    do
    {
        naprav = rand() % 4;
        switch (naprav)
        {
        case RIGHT:
            if (x != 9)
            {
                if (cel[y][x + 1] != MISS && cel[y][x + 1] != HIT && cel[y][x + 1] != COMPNOTSHOOT && cel[y][x + 1] != KILL) zav = 0;
            }
            break;
        case LEFT:
            if (x != 0)
            {
                if (cel[y][x - 1] != MISS && cel[y][x - 1] != HIT && cel[y][x - 1] != COMPNOTSHOOT && cel[y][x - 1] != KILL) zav = 0;
            }
            break;
        case UP:
            if (y != 0)
            {
                if (cel[y - 1][x] != MISS && cel[y - 1][x] != HIT && cel[y - 1][x] != COMPNOTSHOOT && cel[y - 1][x] != KILL) zav = 0;
            }
            break;
        case DOWN:
            if (y != 9)
            {
                if (cel[y + 1][x] != MISS && cel[y + 1][x] != HIT && cel[y + 1][x] != COMPNOTSHOOT && cel[y + 1][x] != KILL) zav = 0;
            }
            break;
        }
    } while (zav);
    switch (naprav)
    {
    case LEFT:
        switch (CShoot(cel, y, x - 1, fekr, sekr))
        {
        case HIT:
            tempY = y;
            tempX = x - 1;
            if (x - 1 == 0) return RIGHT;
            else return LEFT;
            break;
        case KILL:
            return DONE;
            break;
        default:
            return NDONE;
            break;
        }
        break;
    case RIGHT:
        switch (CShoot(cel, y, x + 1, fekr, sekr))
        {
        case HIT:
            tempY = y;
            tempX = x + 1;
            if (x + 1 == 9) return LEFT;
            else return RIGHT;
            break;
        case KILL:
            return DONE;
            break;
        default:
            return NDONE;
            break;
        }
        break;
    case UP:
        switch (CShoot(cel, y - 1, x, fekr, sekr))
        {
        case HIT:
            tempY = y - 1;
            tempX = x;
            if (y - 1 == 0) return DOWN;
            else return UP;
            break;
        case KILL:
            return DONE;
            break;
        default:
            return NDONE;
            break;
        }
        break;
    case DOWN:
        switch (CShoot(cel, y + 1, x, fekr, sekr))
        {
        case HIT:
            tempY = y + 1;
            tempX = x;
            if (y + 1 == 9) return UP;
            else return DOWN;
            break;
        case KILL:
            return DONE;
            break;
        default:
            return NDONE;
            break;
        }
        break;
    }
}

/*Данная функция добивает раненый корабль. Также она переключает направление, если вдруг корабль не убит, а последний выстрел был мимо*/
int FinishHim(int kil[][SIZZE], int pe[][SIZZE], int ve[][SIZZE], int stor)
{
    switch (stor)
    {
    case UP:
        tempY--;
        switch (CShoot(kil, tempY, tempX, pe, ve))
        {
        case 0:
            for (int i = 0; i < SIZZE; i++)
            {
                for (int j = 0; j < SIZZE; j++)
                {
                    if (kil[i][j] == HIT)
                    {
                        tempY = i;
                        tempX = j;
                    }
                }
            }
            return 0;
            break;
        case HIT:
            return HIT;
            break;
        case KILL:
            return KILL;
            break;
        }
        break;
    case DOWN:
        tempY++;
        switch (CShoot(kil, tempY, tempX, pe, ve))
        {
        case 0:
            for (int i = SIZZE - 1; i >= 0; i--)
            {
                for (int j = SIZZE - 1; j >= 0; j--)
                {
                    if (kil[i][j] == HIT)
                    {
                        tempY = i;
                        tempX = j;
                    }
                }
            }
            return 0;
            break;
        case HIT:
            return HIT;
            break;
        case KILL:
            return KILL;
            break;
        }
        break;
    case LEFT:
        tempX--;
        switch (CShoot(kil, tempY, tempX, pe, ve))
        {
        case 0:
            for (int i = 0; i < SIZZE; i++)
            {
                for (int j = 0; j < SIZZE; j++)
                {
                    if (kil[i][j] == HIT)
                    {
                        tempY = i;
                        tempX = j;
                    }
                }
            }
            return 0;
            break;
        case HIT:
            return HIT;
            break;
        case KILL:
            return KILL;
            break;
        }
        break;
    case RIGHT:
        tempX++;
        switch (CShoot(kil, tempY, tempX, pe, ve))
        {
        case 0:
            for (int i = SIZZE - 1; i >= 0; i--)
            {
                for (int j = SIZZE - 1; j >= 0; j--)
                {
                    if (kil[i][j] == HIT)
                    {
                        tempY = i;
                        tempX = j;
                    }
                }
            }
            return 0;
            break;
        case HIT:
            return HIT;
            break;
        case KILL:
            return KILL;
            break;
        }
        break;
    }
}

int main()
{
    char gtch;
    srand(time(0));
    int status = INIT, choose = 0, pointP1 = 0, pointP2 = 0, IIchoose = SEARCH; //переменные переключатели меню
    while (1)
    {
        switch (status)
        {
        case INIT:
            //начало игры, выбор противника
            cout << "\tSEA BATTLE\nDesignation:\n" << char(43) << " - hit\n" << char(35) << " - kill\n" << char(111) << " - miss\n" << char(219) << " - deck of your ship\n" << "\nPress 1 to play vs Computer\nPress 2 to play vs Second player\n";
            cin >> choose;
            if (choose == 1) status = VSCOMP;
            else if (choose == 2) status = VSPLAYER;
            break;
        case VSCOMP:
            //этап создания полей игрока и противника
            system("CLS");
            cout << "\nHow would you like to arrange the ships?\nPress 1 to manually\nPress 2 to automatically\n";
            cin >> choose;
            if (choose == 1)
            {
                ManualPlaceShip(player, playerscr, compscr);
                AutoGenShip(comp);
            }
            else if (choose == 2)
            {
                AutoGenShip(player);
                PvC(player, playerscr);
                DrawingSpace(playerscr, compscr);
                AutoGenShip(comp);
            }
            status = STEPPLAYERVC;
            break;
        case VSPLAYER:
            //этап создания полей двух игроков
            system("CLS");
            cout << "\nPlayer 1, how would you like to arrange the ships?\nPress 1 to manually\nPress 2 to automatically\n";
            cin >> choose;
            if (choose == 1)
            {
                ManualPlaceShip(player, playerscr, plenemyscr);
            }
            else if (choose == 2)
            {
                AutoGenShip(player);
                PvC(player, playerscr);
                DrawingSpace(playerscr, plenemyscr);
            }
            cout << "\nPress any key to clear screen\n";
            gtch = _getch();
            system("CLS");
            cout << "\nPlayer 2, how would you like to arrange the ships?\nPress 1 to manually\nPress 2 to automatically\n";
            cin >> choose;
            if (choose == 1)
            {
                ManualPlaceShip(comp, compscr, coenemyscr);
            }
            else if (choose == 2)
            {
                AutoGenShip(comp);
                PvC(comp, compscr);
                DrawingSpace(compscr, coenemyscr);
            }
            cout << "\nPress any key to clear screen\n";
            gtch = _getch();
            system("CLS");
            cout << "\nPlayer 2 turn away! Player 1, press any key to starting game.\n";
            gtch = _getch();
            status = STEPP1VP2;
            break;
        case STEPP1VP2:
            //ход первого игрока против второго
            system("CLS");
            PvC(player, playerscr);
            DrawingSpace(playerscr, plenemyscr);
            do
            {
                choose = PlayerShoot(plenemyscr, comp, player, playerscr);
                if (choose) pointP1++;
                if (pointP1 == 20) break;
            } while (choose);
            if (pointP1 == 20) status = ENDGAME;
            else
            {
                cout << "\nPress any key to step next player.\n";
                gtch = _getch();
                system("CLS");
                cout << "\nPlayer 1 turn away! Player 2, press any key to continue.\n";
                gtch = _getch();
                status = STEPP2VP1;
            }
            break;
        case STEPP2VP1:
            //ход второго игрока против первого
            system("CLS");
            PvC(comp, compscr);
            DrawingSpace(compscr, coenemyscr);
            do
            {
                choose = PlayerShoot(coenemyscr, player, comp, compscr);
                if (choose) pointP2++;
                if (pointP2 == 20) break;
            } while (choose);
            if (pointP2 == 20) status = ENDGAME;
            else
            {
                cout << "\nPress any key to step next player.\n";
                gtch = _getch();
                system("CLS");
                cout << "\nPlayer 2 turn away! Player 1, press any key to continue.\n";
                gtch = _getch();
                status = STEPP1VP2;
            }
            break;
        case STEPPLAYERVC:
            //ход игрока против компьютера
            do
            {
                choose = PlayerShoot(compscr, comp, player, playerscr);
                if (choose) pointP1++;
                if (pointP1 == 20) break;
            } while (choose);
            if (pointP1 == 20) status = ENDGAME;
            else status = STEPCOMP;
            break;
        case STEPCOMP:
            //ход компьютера против игрока
            /*do
            {
                choose = CompShoot(player, playerscr, compscr);
                if (choose) pointP2++;
                if (pointP2 == 20) break;
            } while (choose);
            if (pointP2 == 20) status = ENDGAME;
            else status = STEPPLAYERVC;*/
            switch (IIchoose)
            {
            case SEARCH:
                choose = CompShoot(player, playerscr, compscr);
                Sleep(1000);
                if (choose == HIT)
                {
                    pointP2++;
                    IIchoose = DIRFIND;
                    status = STEPCOMP;
                }
                else if (choose == KILL)
                {
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                }
                else
                {
                    IIchoose = SEARCH;
                    status = STEPPLAYERVC;
                }
                break;
            case DIRFIND:
                switch (FindingDirection(player, playerscr, compscr))
                {
                case RIGHT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHR;
                    status = STEPCOMP;
                    break;
                case LEFT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHL;
                    status = STEPCOMP;
                    break;
                case UP:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHU;
                    status = STEPCOMP;
                    break;
                case DOWN:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHD;
                    status = STEPCOMP;
                    break;
                case DONE:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                    break;
                case NDONE:
                    Sleep(1000);
                    IIchoose = DIRFIND;
                    status = STEPPLAYERVC;
                    break;
                }
                break;
            case FINISHU:
                switch (FinishHim(player, playerscr, compscr, UP))
                {
                case 0:
                    Sleep(1000);
                    IIchoose = FINISHD;
                    status = STEPPLAYERVC;
                    break;
                case HIT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHU;
                    status = STEPCOMP;
                    break;
                case KILL:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                    break;
                }
                break;
            case FINISHD:
                switch (FinishHim(player, playerscr, compscr, DOWN))
                {
                case 0:
                    Sleep(1000);
                    IIchoose = FINISHU;
                    status = STEPPLAYERVC;
                    break;
                case HIT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHD;
                    status = STEPCOMP;
                    break;
                case KILL:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                    break;
                }
                break;
            case FINISHR:
                switch (FinishHim(player, playerscr, compscr, RIGHT))
                {
                case 0:
                    Sleep(1000);
                    IIchoose = FINISHL;
                    status = STEPPLAYERVC;
                    break;
                case HIT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHR;
                    status = STEPCOMP;
                    break;
                case KILL:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                    break;
                }
                break;
            case FINISHL:
                switch (FinishHim(player, playerscr, compscr, LEFT))
                {
                case 0:
                    Sleep(1000);
                    IIchoose = FINISHR;
                    status = STEPPLAYERVC;
                    break;
                case HIT:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = FINISHL;
                    status = STEPCOMP;
                    break;
                case KILL:
                    Sleep(1000);
                    pointP2++;
                    IIchoose = SEARCH;
                    if (pointP2 == 20) status = ENDGAME;
                    else status = STEPCOMP;
                    break;
                }
                break;
            }
            break;
        case ENDGAME:
            //конец игры
            if (pointP1 == 20) cout << "\nPlayer one win!\n";
            else if (pointP2 == 20) cout << "\nPlayer two/computer win!\n";
            return 0;
            break;
        }
    }
}