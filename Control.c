#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

int a[12][12] = { 0, };

void drawBorad()
{
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) {
			if (a[i][j] == 1)
				printf("*");
		}
	}
}

void gotoxy(int x, int y) {
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

int main() {
	int x = 3;
	int y = 3;
	int chr;

	while (1) {
		gotoxy(x, y);
		chr = _getch();
		if (chr == 0 || chr == 0xe0) {
			chr = _getch();
			if (chr == 72) { //╩С
				y--;
				if (y < 3)
					y = 3;
			}
			else if (chr == 80) { //го
				y++;
				if (y > 14)
					y = 14;
			}
			else if (chr == 75) { //аб
				x--;
				if (x < 3)
					x = 3;
			}
			else if (chr == 77) { //©Л
				x++;
				if (x > 14)
					x = 14;
			}
			system("cls");
			gotoxy(x, y);
		}
		else if (chr == 32)
		{
			if (a[x - 3][y - 3] == 1)
				a[x - 3][y - 3] = 0;
			else if (a[x - 3][y - 3] == 0)
				a[x - 3][y - 3] = 1;
		}
		drawBorad();

	}

}