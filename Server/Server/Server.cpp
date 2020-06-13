// Server.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	int size;
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


	do
	{
		//Nhan check value xem client co tiep tuc hay khong
		mysock.Receive(&continueCheck, sizeof(continueCheck), 0);
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
			unsigned int port = 1234;
			int i;
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
			} while (1);
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
