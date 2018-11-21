include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include<math.h>

#define MAX_X 40
#define MAX_Y 20
#define MIN_X 8
#define MIN_Y 4

#define INTERFACE_X 50
#define INTERFACE_Y 4

#define MAX_BLOCK 16
#define ACTIVE_BLOCK 1
#define INACTIVE_BLOCK 0

int arr[16][16] = { 0, };

int x = MIN_X;
int y = MIN_Y;
int mouse_x = MIN_X;

void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void drawMap()
{
	gotoxy(INTERFACE_X, INTERFACE_Y); printf("		   	  >>HOW TO PLAY<<");
	gotoxy(INTERFACE_X, INTERFACE_Y + 1); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + 2); printf("	l	-	MOVE	:	◁ ▷, △ ▽");
	gotoxy(INTERFACE_X, INTERFACE_Y + 3); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + 4); printf("	l	-ACTIVE  or  INACTIVE : SPACE BAR");
	gotoxy(INTERFACE_X, INTERFACE_Y + 5); printf("	l___________________________________________");
}

void drawBorad()
{
	for (int i = 0; i < MAX_BLOCK; i++) {
		for (int j = 0; j < MAX_BLOCK; j++) {
			gotoxy((2 * i) + MIN_X, j + MIN_Y);
			if ((2 * i) + MIN_X == mouse_x && j + MIN_Y == y) {
				if (arr[i][j] == ACTIVE_BLOCK)
					printf("★");
				else
					printf("☆");
			}
			else if (arr[i][j] == ACTIVE_BLOCK)
				printf("●");
			else
				printf("○");
		}
	}
}

int calcFrequency(int octave, int inx)	//
{
	double do_scale = 32.7032;
	double ratio = pow(2., 1 / 12.), temp;
	int i;

	temp = do_scale * pow(2, octave - 1);
	for (i = 0; i < inx; i++)
	{
		temp = (int)(temp + 0.5);
		temp *= ratio;
	}
	return (int)temp;
}

void makeSound(int row) //
{
	int freq[8];
	int index[] = { 0, 2, 4, 5, 7, 9, 11, 12 };
	for (int i = 0; i < 8; i++)
		freq[i] = calcFrequency(4, index[i]);

	for (int i = 0; i < 8; i++)
	{
		gotoxy(MIN_X + i * 2, MIN_Y - 1);
		if (row == i)
			printf("1 ");
		else
			printf("0 ");

		if(arr[i][row] == ACTIVE_BLOCK)
			Beep(freq[i], 100);
	}
}

int main() {
	int chr;
	int arr_row = 0;

	drawBorad();
	drawMap();
	while (1) {
		if (arr_row == 16)	arr_row = 0;	//

		makeSound(arr_row++);		//

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
			if (arr[x - MIN_X][y - MIN_Y] == ACTIVE_BLOCK)
				arr[x - MIN_X][y - MIN_Y] = INACTIVE_BLOCK;
			else if (arr[x - MIN_X][y - MIN_Y] == INACTIVE_BLOCK)
				arr[x - MIN_X][y - MIN_Y] = ACTIVE_BLOCK;
		}
		drawBorad();
		drawMap();
		gotoxy(0, 20);
		printf("x : %d, mouse_x : %d\n", x, mouse_x);
		printf("y : %d", y);
		gotoxy(mouse_x, y);
	}

}
