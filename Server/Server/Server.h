#pragma once
#include "dirent.h"
#include "resource.h"
#include<fstream>
#include<iostream>
using namespace std;

const int upload = 1;
const int download = 2;
const char databasePath[] = "Database";

void ListFile(char* path);