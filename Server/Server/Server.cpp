// Server.cpp : Defines the entry point for the console application.
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"

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
	if (pDIR = opendir(databasePath)) {
		while (entry = readdir(pDIR)) {
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
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

// The one and only application object
CWinApp theApp;

using namespace std;

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

DWORD WINAPI function_cal(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);

	int choice,Usize, Psize,correct = 0, continueCheck = 0;
	char* User;
	char* Password;

	fflush(stdin);
	mysock.Receive((char*)&choice, sizeof(int), 0);
Loop:
	//Nhan kich thuoc tai khoan tu client
	mysock.Receive((char*)&Usize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
	User = new char[Usize + 1];
	//Nhan tai khoan
	mysock.Receive((char*)User, Usize, 0);

	//Nhan kich thuoc mat khau tu client
	mysock.Receive((char*)&Psize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
	Password = new char[Psize + 1];
	//Nhan mat khau
	mysock.Receive((char*)Password, Psize, 0);
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
	mysock.Send(&correct, sizeof(correct), 0);
	if (correct == 0 && choice == 1)
	{
		goto Loop;
	}
	else if (correct == 0 && choice == 2)
	{
		cout << User << " da tao tai khoan thanh cong dang trong tinh trang dang nhap" << endl;
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


	do{
		//Nhan check value xem client co tiep tuc hay khong
		mysock.Receive(&continueCheck, sizeof(continueCheck), 0);
		mysock.Send(&isOperating, sizeof(isOperating), 0);
		if (isOperating == false) {
			isOperating = true;
			if (continueCheck == upload) {
				isOperating = true;
				bool exist = false;
				mysock.Receive(&exist, sizeof(exist), 0);
				if (exist == true) {
					char fileName[100];
					char path[100] = "Database/";
					int length = 0;

					// tao path "Database/fileName"
					mysock.Receive(&length, sizeof(length), 0);
					mysock.Receive(fileName, length, 0);
					fileName[length] = '\0';
					int count = 0;
					int dotPos = 0;
					duplicateFile((char*)databasePath, fileName, dotPos, count);
					strcat_s(path, fileName);

					// fstream cua file duoc upload
					fstream output;
					output.open(path, ios::out | ios::binary);
					int buffLength = 0; // do dai doan bin moi lan gui

					while (true) {
						mysock.Receive(&buffLength, sizeof(buffLength), 0);
						if (buffLength == 0) break;
						else {
							char* buff = new char[buffLength];
							mysock.Receive(buff, buffLength, 0);
							output.write(buff, buffLength);
							delete[] buff;
						}
					}
					output.close();
					cout << "File " << fileName << " da duoc " << User << " upload len Database.\n";
				}
			}
			else if (continueCheck == download) {
				cout << User << " muon download file tu server\n" << endl;
				DIR* pDIR;
				struct dirent* entry;
				if (pDIR = opendir(databasePath)) {
					while (entry = readdir(pDIR)) {
						if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
							mysock.Send(&(entry->d_namlen), sizeof(int), 0);
							mysock.Send(&(entry->d_name), entry->d_namlen, 0);
						}
					}
					int zero = 0; // thoat vong lap ben phia client
					mysock.Send(&zero, sizeof(zero), 0);
					closedir(pDIR);
				}
				char path[100] = "Database/";
				char fileName[100];
				int nameLength = 0;
				mysock.Receive(&nameLength, sizeof(nameLength), 0);
				mysock.Receive(fileName, nameLength, 0);
				fileName[nameLength] = '\0';
				cout << User << " muon tai file " << fileName << endl;
				strcat_s(path, fileName);

				ifstream f;
				bool exist = false;
				f.open(path, ios::in | ios::binary);
				if (f.good()) {
					exist = true;
					mysock.Send(&exist, sizeof(exist), 0);
					int size = 1024 * 1024;
					char* buff = new char[size];
					int buffLength = 0;
					while (!f.eof()) {
						f.read((char*)buff, size);
						buffLength = f.gcount();
						buff[buffLength] = '\0';
						mysock.Send(&buffLength, sizeof(buffLength), 0);
						mysock.Send(buff, buffLength, 0);
					}
					delete[] buff;
					buffLength = 0;
					mysock.Send(&buffLength, sizeof(buffLength), 0);
					cout << User << " da tai file " << fileName << endl;
				}
				else {
					mysock.Send(&exist, sizeof(exist), 0);
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
	int nRetCode = 0;
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
			CSocket server,s;
			AfxSocketInit(NULL);
			DWORD threadID;
			HANDLE threadStatus;

			server.Create(port);
			do {
				cout << "Server lang nghe ket noi tu client" << endl;
				server.Listen();
				server.Accept(s);
				//Khoi tao con tro Socket
				SOCKET* hConnected = new SOCKET();
				//Chuyen doi CSocket thanh Socket
				*hConnected = s.Detach();
				//Khoi tao thread tuong ung voi moi client Connect vao server.
				//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
				threadStatus = CreateThread(NULL, 0, function_cal, hConnected, 0, &threadID);
			} while (true);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	getchar();
	return nRetCode;
}
