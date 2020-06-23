#pragma once
#include "dirent.h"
#include "resource.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <sstream>
#include <conio.h>

using namespace std;

const int colWidth = 60;
const int colHeight = 30;

static int x3 = 2 * colWidth, y3 = 1, x2 = colWidth, y2 = 1, x = 1, y = 1;

#define port 1234
#define upload 1
#define download  2
#define max_file_size  1024*1024*200

//Display
void gotoxy(int x, int y);
bool checkPos(int x, int y);
void displayClient();
void clearCol(int colNum);
void printStringXY(const char* str, int colNum);

//Process
void handleClose();
int fileSize(char* path);