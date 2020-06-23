#pragma once
#include "dirent.h"
#include "resource.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include <conio.h>
#include <windows.h>

#define port 1234
#define upload 1
#define download 2
#define max_file_size 1024 * 1024 * 200
using namespace std;

const int colWidth = 40;
const int colHeight = 30;

static int x3 = 2 * colWidth, y3 = 1, x2 = colWidth, y2 = 1, x = 1, y = 1, status = 0;

const char databasePath[] = "Database";
static bool isOperating = false;
static char UserC[100];

//Display
void gotoxy(int x, int y);
bool checkPos(int x, int y);
void printStringXY(const char* str, int colNum);
void displayServer();
void clearCol(int colNum);

//Server Process
int fileSize(char* path);
int Check(char check[100], char right[100]);
void duplicateFile(char* path, char* fileName, int dotPos, int& count);
int handleClose();