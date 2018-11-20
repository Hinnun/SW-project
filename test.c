#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define MAX_X 40
#define MAX_Y 20
#define MIN_X 8
#define MIN_Y 4

#define INTERFACE_X 50
#define INTERFACE_Y 4

#define MAX_BLOCK 16
#define ACTIVE_BLOCK 1
#define INACTIVE_BLOCK 0

int a[16][16] = { 0, };

int x = MIN_X;
int y = MIN_Y;
int mouse_x = MIN_X;

void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void drawMap()
{
	gotoxy(INTERFACE_X, INTERFACE_Y    ); printf("		   	  >>HOW TO PLAY<<");
	gotoxy(INTERFACE_X, INTERFACE_Y + 1); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + 2); printf("	l	-	MOVE	:	¡ç ¡æ, ¡è ¡é");
	gotoxy(INTERFACE_X, INTERFACE_Y + 3); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + 4); printf("	l	-ACTIVE  or  INACTIVE : SPACE BAR");
	gotoxy(INTERFACE_X, INTERFACE_Y + 5); printf("	l___________________________________________");
}

void drawBorad()
{
	for (int i = 0; i < MAX_BLOCK; i++) {
		for (int j = 0; j < MAX_BLOCK; j++) {
			gotoxy((2 * i) + MIN_X, j + MIN_Y);
			if (a[i][j] == ACTIVE_BLOCK)
				printf("¡á");
			else
				printf("¡à");
		}
	}
}

int main() {
	int chr;

	drawBorad();

	while (1) {
		gotoxy(mouse_x, y);
		chr = _getch();
		if (chr == 0 || chr == 0xe0) {
			chr = _getch();

			if (chr == 72) {
				y--;
				if (y < MIN_Y) {
					y = MIN_Y;
				}
			}
			else if (chr == 80) {
				y++;
				if (y > MAX_Y - 1) {
					y = MAX_Y - 1;
				}
			}
			else if (chr == 75) {
				x--;
				mouse_x -= 2;
				if (mouse_x < MIN_X) {
					mouse_x = MIN_X;
					x = MIN_X;
				}
			}
			else if (chr == 77) {
				x++;
				mouse_x += 2;
				if (mouse_x > MAX_X - 2) {
					mouse_x = MAX_X - 2;
					x = MAX_X - MAX_BLOCK - 1;
				}
			}
			system("cls");
			gotoxy(mouse_x, y);
		}
		else if (chr == 32)
		{
			if (a[x - MIN_X][y - MIN_Y] == ACTIVE_BLOCK)
				a[x - MIN_X][y - MIN_Y] = INACTIVE_BLOCK;
			else if (a[x - MIN_X][y - MIN_Y] == INACTIVE_BLOCK)
				a[x - MIN_X][y - MIN_Y] = ACTIVE_BLOCK;
			_getch();
		}
		drawBorad();
		drawMap();
		gotoxy(0, 20);
		printf("x : %d, mouse_x : %d\n", x, mouse_x);
		printf("y : %d", y);
		gotoxy(mouse_x, y);
	}

}