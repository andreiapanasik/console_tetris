#include <stdio.h>
#include <stdlib.h>
#include "tetris.h"
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#define TETRIS_PIECES (sizeof(blocks)/sizeof(struct tetris_block))
#define GAME_LEVELS (sizeof(levels)/sizeof(struct game_level))

struct game_level
{
    int score;
    int nsec;
};

struct game
{
    char **game;
    int w;
    int h;
    int level;
    int gameover;
    int score;
    struct tetris_block
    {
        char data[5][5];
        int w;
        int h;
    } current;
    int x;
    int y;
};

struct tetris_block blocks[] =
        {
                { { "##", "##" }, 2, 2 },
                { { " X ", "XXX" }, 3, 2 },
                { { "@@@@"}, 4, 1 },
                { { "OO", "O ", "O " }, 2, 3 },
                { { "&&", " &", " &" }, 2, 3 },
                { { "ZZ ", " ZZ" }, 3, 2 }
        };

struct game_level levels[]=
        {
                { 0, 1200000 },
                { 1500, 900000 },
                { 8000, 700000 },
                { 20000, 500000 },
                { 40000, 400000 },
                { 75000, 300000 },
                { 100000, 200000 }
        };

struct termios save;

void areaCleanupIo()
{
    tcsetattr(fileno(stdin) ,TCSANOW, &save);
}

void areaSetIoconfig()
{
    struct termios custom;
    int fd = fileno(stdin);
    tcgetattr(fd, &save);
    custom = save;
    custom.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(fd, TCSANOW, &custom);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0)|O_NONBLOCK);
}

void gameInit(struct game *t, int w, int h)
{
    t->level = 1;
    t->score = 0;
    t->gameover = 0;
    t->w = w;
    t->h = h;
    t->game = malloc(sizeof(char *) * w);
    for (int x = 0; x < w; x++)
    {
        t->game[x] = malloc(sizeof(char)*h);
        for (int y = 0; y < h; y++) t->game[x][y] = ' ';
    }
}

void gameClean(struct game *t)
{
    for (int x = 0; x < t->w; x++) free(t->game[x]);
    free(t->game);
}

void areaPrint(struct game *t)
{

    for (int x = 0; x < 30; x++) printf("\n");
    printf("[LEVEL: %d | SCORE: %d]\n", t->level, t->score);
    for (int x = 0; x < 2 * t->w + 2; x++) printf("-");
    printf("\n");

    for (int y = 0; y < t->h; y++)
    {
        printf ("|");
        for (int x = 0; x < t->w; x++)
        {
            if (x >= t->x
                && y >= t->y
                && x < (t->x + t->current.w) && y < (t->y + t->current.h)
                && t->current.data[y - t->y][x - t->x] != ' ')
            { printf("%c ", t->current.data[y - t->y][x - t->x]); }
            else
                printf("%c ", t->game[x][y]);
        }
        printf ("|\n");
    }
    for (int x = 0; x < 2 * t->w + 2; x++) printf("-");
    printf("\n");
}

int hitCheck(struct game *t)
{
    int X, Y;
    struct tetris_block block = t->current;
    for (int x = 0; x < block.w; x++)
        for (int y = 0; y < block.h; y++)
        {
            X = t->x + x;
            Y = t->y + y;
            if (X < 0 || X >= t->w) return 1;

            if (block.data[y][x] != ' ')
            {
                if ((Y >= t->h)
                    || (X >= 0 && X < t->w && Y >= 0 && t->game[X][Y] != ' '))
                { return 1; }
            }
        }
    return 0;
}

void newBlock(struct game *t)
{
    t->current = blocks[random() % TETRIS_PIECES];
    t->x = (t->w / 2) - (t->current.w / 2);
    t->y = 0;
    if (hitCheck(t)) t->gameover = 1;
}

void printBlock(struct game *t)
{
    struct tetris_block block = t->current;
    for (int x = 0; x < block.w; x++)
        for (int y = 0; y < block.h; y++)
        {
            if (block.data[y][x] != ' ')
                t->game[t->x + x][t->y + y] = block.data[y][x];
        }
}

void rotateBlock(struct game *t)
{
    struct tetris_block block = t->current;
    struct tetris_block temp = block;
    int x,y;
    block.w = temp.h;
    block.h = temp.w;
    for (x = 0; x < temp.w; x++)
        for (y = 0; y < temp.h; y++)
            block.data[x][y] = temp.data[temp.h-y-1][x];
    x = t->x;
    y = t->y;
    t->x -= (block.w - temp.w)/2;
    t->y -= (block.h - temp.h)/2;
    t->current = block;
    if (hitCheck(t))
    {
        t->current = temp;
        t->x = x;
        t->y = y;
    }
}

void gravityBlock(struct game *t)
{
    t->y++;
    if (hitCheck(t))
    {
        t->y--;
        printBlock(t);
        newBlock(t);
    }
}

void allBlocksFall(struct game *t, int l)
{
    for (int y = l; y > 0; y--)
    {
        for (int x = 0; x < t->w; x++)
            t->game[x][y] = t->game[x][y-1];
    }
    for (int x = 0; x < t->w; x++)
        t->game[x][0] = ' ';
}

void checkLines(struct game *t)
{
    int x, y, l;
    int p = 100;
    for (y = t->h - 1; y >= 0; y--)
    {
        l = 1;
        for (x = 0; x < t->w && l; x++)
            if (t->game[x][y] == ' ') l = 0;

        if (l)
        {
            t->score += p;
            p *= 2;
            allBlocksFall(t, y);
            y++;
        }
    }
}

int gameLevel(struct game *t)
{
    for (int i = 0; i < GAME_LEVELS; i++)
    {
        if (t->score >= levels[i].score) t->level = i+1;
        else break;
    }
    return levels[t->level-1].nsec;
}


void palyGame(int w, int h)
{
    struct timespec tm;
    struct game t;
    char cmd;
    int count = 0;
    areaSetIoconfig();
    gameInit(&t, w, h);
    srand(time(NULL));

    tm.tv_sec = 0;
    tm.tv_nsec = 1000000;

    newBlock(&t);
    while (!t.gameover)
    {
        nanosleep(&tm, NULL);
        count++;
        if (count % 100 == 0)
            areaPrint(&t);

        if (count % 350 == 0)
        {
            gravityBlock(&t);
            checkLines(&t);
        }
        while ((cmd = getchar()) > 0)
        {
            switch (cmd)
            {
                case 'a':
                    t.x--;
                    if (hitCheck(&t))
                        t.x++;
                    break;
                case 'd':
                    t.x++;
                    if (hitCheck(&t))
                        t.x--;
                    break;
                case 's':
                    gravityBlock(&t);
                    break;
                case 'w':
                    rotateBlock(&t);
                    break;
                case 'q':
                    exit(0);
                    break;
            }
        }
        tm.tv_nsec = gameLevel(&t);
    }

    areaPrint(&t);
    printf("*** GAME OVER ***\n");

    gameClean(&t);
    areaCleanupIo();
}
