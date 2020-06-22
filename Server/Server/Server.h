#pragma once
#include "dirent.h"
#include "resource.h"
#include <fstream>
#include <iostream>
#include <sstream>
#define port 1234
#define upload 1
#define download 2
#define max_file_size 1024 * 1024 * 200
using namespace std;



static int status = 0;
const char databasePath[] = "Database";
bool isOperating = false;
char UserC[100];

//Console Windows
void gotoxy(int x, int y);
void printStringXY(const char* str, int colNum);
void displayServer();

//Process
int fileSize(char* path);
int Check(char check[100], char right[100]);
void duplicateFile(char* path, char* fileName, int dotPos, int& count);
int handleClose();