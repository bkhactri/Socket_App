#pragma once
#include "dirent.h"
#include "resource.h"
#include <iostream>
#include <fstream>
using namespace std;

#define port 1234
#define upload 1
#define download  2
#define max_file_size  1024*1024*200


void handleClose();
char sAdd[1000];
int fileSize(char* path);


