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

int handleClose()
{
	int bESCPressed = 0;
	do
	{
		bESCPressed = (_getch() == 27);
	} while (!bESCPressed);

	return 1;
}

DWORD WINAPI threadFunction_send_flag(LPVOID arg)
{
	int flag = 0;
	int size;
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket server;
	//Chuyen ve lai CSocket
	server.Attach(*hConnected);
	do
	{
		if (status == 1)
		{
			flag = 1;
			server.Send(&flag, sizeof(flag), 0);
		}
		else if (status == 2)
		{
			flag = 2;
			server.Send(&flag, sizeof(flag), 0);
			size = strlen(UserC);
			server.Send(&size, sizeof(size), 0);
			// Gui di tai khoan voi do dai la size
			server.Send(UserC, size, 0);
			status = 0;
		}
		else if (status == 3)
		{
			flag = 3;
			server.Send(&flag, sizeof(flag), 0);
			size = strlen(UserC);
			server.Send(&size, sizeof(size), 0);
			// Gui di tai khoan voi do dai laUsize
			server.Send(UserC, size, 0);
			status = 0;
		}
	} while (flag != 1);

	exit(0);
	cin.clear();
	cin.ignore();
	return 0;
}

DWORD WINAPI threadFunction_stop_key(LPVOID arg)
{
	do
	{
		if (handleClose() == 1)
		{
			status = 1;
			cout << "Server da ngat ket noi" << endl;
		}
	} while (true);
	return 0;
}

DWORD WINAPI threadFunction_stop_each_client(LPVOID arg)
{
	int i = 0;
	DWORD threadID1;
	HANDLE threadClient;
	unsigned int port1 = 1235;
	CSocket server, s;
	AfxSocketInit(NULL);
	server.Create(port1);
	do
	{
		server.Listen();
		server.Accept(s);
		SOCKET* hConnected = new SOCKET();
		//Chuyen doi CSocket thanh Socket
		*hConnected = s.Detach();
		//Khoi tao thread tuong ung voi moi client Connect vao server.
		threadClient = CreateThread(NULL, 0, threadFunction_send_flag, hConnected, 0, &threadID1);
	} while (true);

	return 0;
}

DWORD WINAPI threadFunction_handle_each_client(LPVOID arg)
{
	int LogStatus = 0;
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
				status = 2;
				strcpy(UserC, User);
				LogStatus = 1;
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
		LogStatus = 1;
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
		server.Receive((char*)&continueCheck, sizeof(int), 0);
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
					char* buff = new char[buffLength + 1];
					server.Receive((char*)buff, buffLength, 0);
					buff[buffLength] = '\0';
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
				server.Receive((char*)&nameLength, sizeof(nameLength), 0);
				server.Receive((char*)fileName, nameLength, 0);
				fileName[nameLength] = '\0';
				strcat_s(path, fileName);

				ifstream f;
				bool exist = false;
				f.open(path, ios::in | ios::binary);
				if (f.good())
				{
					exist = true;
					server.Send(&exist, sizeof(exist), 0);
					char* buff = new char[max_file_size + 1];
					for (int i = 0; i < max_file_size; i++) {
						buff[i] = '\0';
					}
					int buffLength = 0;
					f.read((char*)buff, max_file_size);
					buffLength = f.gcount();
					buff[buffLength] = '\0';
					server.Send(&buffLength, sizeof(buffLength), 0);
					server.Send(buff, buffLength, 0);
					delete[] buff;
					cout << User << " da tai file " << fileName << endl;
					f.seekg(0, ios::beg);
				}
				else
				{
					server.Send(&exist, sizeof(exist), 0);
					cout << User << " donwnload file khong ton tai" << endl;
				}
				f.close();
			}
			
		}
		continueCheck = 99;
		isOperating = false;
	} while (continueCheck != 0);
	status = 3;
	strcpy(UserC, User);
	if (LogStatus == 0)
	{
		cout << "Unknown client da dang xuat" << endl;
	}
	else if (LogStatus == 1)
	{
		cout << User << " da dang xuat" << endl;
	}
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
			threadStop = CreateThread(NULL, 0, threadFunction_stop_key, NULL, 0, &threadID3);
			threadStopEachClient = CreateThread(NULL, 0, threadFunction_stop_each_client, NULL, 0, &threadID2);
			do
			{
				//threadStopEachClient = CreateThread(NULL, 0, threadFunction_stop_each_client, NULL, 0, &threadID2);
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