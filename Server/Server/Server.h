#pragma once
#include "dirent.h"
#include "resource.h"
#include<fstream>
#include<iostream>
#define port 1234
#define upload 1
#define download 2
#define max_file_size 1024 * 1024 * 200
using namespace std;
const char databasePath[] = "Database";
bool isOperating = false;
int fileSize(char* path);
void ListFile(char* path);
void duplicateFile(char* path, char* fileName, int dotPos, int& count);