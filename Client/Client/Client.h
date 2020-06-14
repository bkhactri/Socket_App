#pragma once
#include "dirent.h"
#include "resource.h"
#include <iostream>
#include <fstream>
using namespace std;

const int upload = 1;
const int download = 2;
const int max_file_size = 1024*1024*200;

int fileSize(char* path);



