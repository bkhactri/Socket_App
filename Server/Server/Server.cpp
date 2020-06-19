// Server.cpp : Defines the entry point for the console application.
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"
#include<conio.h>

// The one and only application object
CWinApp theApp;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int fileSize(char* path) {
	ifstream f(path, ios::binary);
	f.seekg(0, ios::end);
	int file_size = f.tellg();
	f.seekg(0, ios::beg);
	f.close();
	return file_size;
}

void duplicateFile(char* path, char* fileName, int dotPos, int& count) {
	DIR* pDIR;
	struct dirent* entry;
	if (pDIR = opendir(databasePath))
	{
		while (entry = readdir(pDIR)) 
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
			{
				if (strcmp(fileName, entry->d_name) == 0) {
					// Lay vi tri '.'
					int length = strlen(fileName);
					int newDotPos = 0;
					for (int i = length - 1; i >= 0; i--) {
						if (fileName[i] == '.') {
							newDotPos = i;
							break;
						}
					}
					if (count == 0) dotPos = newDotPos;
					count++;
					char adder[10] = " (";
					char dupTimes[10];
					_itoa(count, dupTimes, 10);
					strcat(adder, dupTimes);
					strcat(adder, (char*)")");
					strcat(adder, fileName + newDotPos);
					strcpy(fileName + dotPos, adder);
					duplicateFile(path, fileName, dotPos, count);
					break;
				}
			}
		}
		closedir(pDIR);
	}
}

int Check(char C[100], char R[100])
{
	int s1 = strlen(C);
	int s2 = strlen(C);
	if (s1 != s2) return 0;
	else
	{
		for (int i = 0; i < s1; i++)
		{
			if (C[i] != R[i])
			{
				return 0;
			}
		}
		return 1;
	}
}

void handleClose()
{
	int bESCPressed = 0;
	do
	{
		bESCPressed = (_getch() == 27);
	} while (!bESCPressed);
}

DWORD WINAPI threadFunction_stop(LPVOID arg)
{
	do
	{
		if (status == 1)
		{
			cout << "Server da ngat ket noi" << endl;
			exit(0);
		}
	} while (true);
	return 0;
}

DWORD WINAPI threadFunction_stop_each_client(LPVOID arg)
{
	int flag;
	unsigned int port1 = 1235;
	HMODULE hModule = ::GetModuleHandle(NULL);
	CSocket server, s;
	AfxSocketInit(NULL);
	server.Create(port1);
	server.Listen();
	server.Accept(s);
	handleClose();
	flag = 1;
	s.Send(&flag, sizeof(flag), 0);
	server.Close();
	status = 1;
	return 0;
}

DWORD WINAPI threadFunction_handle_each_client(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket server;
	//Chuyen ve lai CSocket
	server.Attach(*hConnected);

	int choice, Usize = 0, Psize = 0, correct = 0, continueCheck = 0;
	char* User;
	char* Password;

	fflush(stdin);
	server.Receive((char*)&choice, sizeof(int), 0);
Loop:
	//Nhan kich thuoc tai khoan tu client
	server.Receive((char*)&Usize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
	User = new char[Usize + 1];
	//Nhan tai khoan
	server.Receive((char*)User, Usize, 0);

	//Nhan kich thuoc mat khau tu client
	server.Receive((char*)&Psize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
	Password = new char[Psize + 1];
	//Nhan mat khau
	server.Receive((char*)Password, Psize, 0);
	User[Usize] = '\0';
	Password[Psize] = '\0';

	fstream f;
	f.open("User.bin", ios::binary || ios::in);
	if (f.fail()) { return 0; }
	//f >> size;
	while (!f.eof()) //for (int i = 0; i < size; i++)
	{
		char* C1 = new char[100];
		char* C2 = new char[100];
		f.getline(C1, 100);
		f.getline(C2, 100);

		if (Check(User, C1) == 1)
		{
			if ((choice == 1 && Check(Password, C2) == 1))
			{
				correct = 1;
				cout << User << " da dang nhap" << endl;
				break;
			}
			if (choice == 2)
			{
				correct = 1;
				break;
			}
		}
	}
	f.close();
	server.Send(&correct, sizeof(correct), 0);
	if (correct == 0 && choice == 1)
	{
		goto Loop;
	}
	else if (correct == 0 && choice == 2)
	{
		cout << User << " da tao tai khoan thanh cong" << endl;
		f.open("User.bin", ios::app);
		if (f.fail()) { return 0; }
		f << User << endl;
		f << Password << endl;
		f.close();
	}
	else if (correct == 1 && choice == 2)
	{
		correct = 0;
		goto Loop;
	}

	do
	{
		//Nhan check value xem client co tiep tuc hay khong
		server.Receive(&continueCheck, sizeof(continueCheck), 0);
		server.Send(&isOperating, sizeof(isOperating), 0);
		if (isOperating == false)
		{
			isOperating = true;
			if (continueCheck == upload)
			{
				isOperating = true;
				bool exist = false;
				server.Receive(&exist, sizeof(exist), 0);
				if (exist == true)
				{
					char fileName[100];
					char path[100] = "Database/";
					int length = 0;
					char* buff;

					// tao path "Database/fileName"
					server.Receive(&length, sizeof(length), 0);
					server.Receive(fileName, length, 0);
					fileName[length] = '\0';
					int count = 0;
					int dotPos = 0;
					duplicateFile((char*)databasePath, fileName, dotPos, count);
					strcat_s(path, fileName);

					// fstream cua file duoc upload
					fstream output;
					output.open(path, ios::out | ios::binary);
					int buffLength = 0; // do dai doan bin moi lan gui
					server.Receive((char*)&buffLength, sizeof(int), 0);
					cout << buffLength << endl;
					buff = new char[buffLength + 1];
					buff[buffLength] = '\0';
					server.Receive((char*)buff, buffLength, 0);
					output.write(buff, buffLength);
					delete[] buff;
					output.close();
					cout << "File " << fileName << " da duoc " << User << " upload len Database.\n";
				}
			}
			else if (continueCheck == download)
			{
				DIR* pDIR;
				struct dirent* entry;
				if (pDIR = opendir(databasePath))
				{
					while (entry = readdir(pDIR))
					{
						if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
						{
							server.Send(&(entry->d_namlen), sizeof(int), 0);
							server.Send(&(entry->d_name), entry->d_namlen, 0);
						}
					}
					int zero = 0; // thoat vong lap ben phia client
					server.Send(&zero, sizeof(zero), 0);
					closedir(pDIR);
				}
				char path[100] = "Database/";
				char fileName[100];
				int nameLength = 0;
				server.Receive(&nameLength, sizeof(nameLength), 0);
				server.Receive(fileName, nameLength, 0);
				fileName[nameLength] = '\0';
				cout << User << " muon tai file " << fileName << endl;
				strcat_s(path, fileName);

				ifstream f;
				bool exist = false;
				f.open(path, ios::in | ios::binary);
				if (f.good())
				{
					exist = true;
					server.Send(&exist, sizeof(exist), 0);

					char* buff = new char[max_file_size + 1];
					buff[max_file_size] = '\0';
					int buffLength = 0;
					f.read((char*)buff, max_file_size);
					buffLength = f.gcount();
					buff[buffLength] = '\0';
					server.Send((char*)&buffLength, sizeof(buffLength), 0);
					server.Send((char*)buff, buffLength, 0);
					delete[] buff;
					cout << User << " da tai file " << fileName << endl;
				}
				else
				{
					server.Send(&exist, sizeof(exist), 0);
					cout << "Khong ton tai file " << fileName << endl;
				}
				f.close();
			}
			isOperating = false;
		}
	} while (continueCheck);
	cout << User << " da dang xuat" << endl;
	delete hConnected;
	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	long long int i = 1;
	int nRetCode = 0;
	DWORD threadID1, threadID2, threadID3;
	HANDLE threadClient, threadStopEachClient, threadStop;
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.

			CSocket server, joiner;
			AfxSocketInit(NULL);
			server.Create(port);

			//Khoi tao thread de nam bat su kien server bi van de
			cout << "Nhan ESC de release moi Client va tat Server" << endl;
			threadStop = CreateThread(NULL, 0, threadFunction_stop, NULL, 0, &threadID2);
			do
			{
				threadStopEachClient = CreateThread(NULL, 0, threadFunction_stop_each_client, NULL, 0, &threadID2);
				Sleep(15);
				cout << "S" << "[" << i << "]" << ": Server lang nghe ket noi tu client" << endl;
				server.Listen();
				server.Accept(joiner);
				//Khoi tao con tro Socket
				SOCKET* hConnected = new SOCKET();
				//Chuyen doi CSocket thanh Socket
				*hConnected = joiner.Detach();
				//Khoi tao thread tuong ung voi moi client Connect vao server.
				threadClient = CreateThread(NULL, 0, threadFunction_handle_each_client, hConnected, 0, &threadID1);
				i++;
			} while (true);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	return nRetCode;
}