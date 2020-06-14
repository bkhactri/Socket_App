#pragma once
#include "dirent.h"
#include "resource.h"
#include<fstream>
#include<iostream>
using namespace std;

const int upload = 1;
const int download = 2;
const int max_file_size = 1024 * 1024 * 200;
const char databasePath[] = "Database";
int fileSize(char* path);
void ListFile(char* path);
void duplicateFile(char* path, char* fileName, int dotPos, int& count);