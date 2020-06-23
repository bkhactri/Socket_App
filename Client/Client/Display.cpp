#include "Client.h"

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}

bool checkPos(int x, int y)
{
	if (x == 0 || x == colWidth - 1 || x == 2 * colWidth - 1) return 0;
	if (y == 0 || y == colHeight - 1) return 0;
	return 1;
}

void displayClient()
{
	system("color 0A");
	char c;
	for (size_t i = 0; i < colHeight; i++)
	{
		for (size_t j = 0; j < 2 * colWidth; j++)
		{
			char c;
			if (i == 0)
			{
				if (j == 0) c = 201;
				else if (j == 2 * colWidth - 1) c = 187;
				else if (j == colWidth - 1) c = 203;
				else c = 205;
			}
			else if (i == colHeight - 1)
			{
				if (j == 0) c = 200;
				else if (j == 2 * colWidth - 1) c = 188;
				else if (j == colWidth - 1) c = 202;
				else c = 205;
			}
			else
			{
				if (j == 0 || j == 2 * colWidth - 1 || j == colWidth - 1) c = 186;
				else c = 32;
			}
			cout << c;
		}
		cout << endl;
	}
}

void clearCol(int colNum)
{
	switch (colNum)
	{
	case 1:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = 1; b < colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x = 1; y = 1;
		break;
	case 2:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = colWidth; b < 2 * colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x2 = colWidth; y2 = 1;
		break;
	case 3:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = 2 * colWidth; b < 3 * colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x3 = 2 * colWidth; y3 = 1;
		break;
	}
}

void printStringXY(const char* str, int colNum) // colNum la so thu tu cua cot can in
{
	int i = 0;
	switch (colNum)
	{
	case 1:
	{
		for (i; i < strlen(str); i++)
		{
			if (!checkPos(x + 1, y))
			{
				if (y < colHeight - 2) y++;
				else
				{
					clearCol(colNum);
					y = 1;
				}
				x = 1;
			}
			if (str[i] == '\n')
			{
				y++;
				x = 1;
			}
			else
			{
				gotoxy(x, y);
				cout << str[i];
				x++;
			}
		}
		break;
	}
	case 2:
	{
		for (i; i < strlen(str); i++)
		{
			if (!checkPos(x2 + 1, y2))
			{
				if (y2 < colHeight - 2) y2++;
				else
				{
					clearCol(colNum);
					y2 = 1;
				}
				x2 = colWidth;
			}
			if (str[i] == '\n')
			{
				y2++;
				x2 = colWidth;
			}
			else
			{
				gotoxy(x2, y2);
				cout << str[i];
				x2++;
			}
		}
		break;
	}
	}
}