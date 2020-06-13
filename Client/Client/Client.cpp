// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

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
			CSocket client;
			char sAdd[1000];
			unsigned int port = 1234; //Cung port voi server
			AfxSocketInit(NULL);

			//1. Tao socket
			client.Create();

			// Nhap dic chi IP cua server
			cout << "Nhap dia chi IP cua server: ";
			gets_s(sAdd);
			int correct = 0, continueCheck = 0, choice;
			if (client.Connect(CA2W(sAdd), port))
			{
				char User[100];
				char Password[100];
				int Usize, Psize;
				cout << "Client da ket noi toi server" << endl;
				cout << "1.Dang nhap" << endl;
				cout << "2.Tao tai khoan" << endl;
				cout << "Lua chon cua ban: "; cin >> choice;
				cin.ignore();
				client.Send(&choice, sizeof(choice), 0);
				if (choice == 1)
				{
				Loop:
					cout << "Nhap vao tai khoan: ";
					cin.getline(User, 100);

					cout << "Nhap vao mat khau: ";
					cin.getline(Password, 100);

					Usize = strlen(User);
					Psize = strlen(Password);

					// Gui di do dai tai khoan de Server biet duoc do dai nay
					client.Send(&Usize, sizeof(Usize), 0);
					// Gui di tai khoan voi do dai la Usize
					client.Send(User, Usize, 0);

					// Gui di do dai tai khoan de Server biet duoc do dai nay
					client.Send(&Psize, sizeof(Psize), 0);
					// Gui di tai khoan voi do dai la Usize
					client.Send(Password, Psize, 0);

					//Nhan ket qua tra ve tu server
					client.Receive((char*)&correct, sizeof(int), 0);

					if (choice == 2)
					{
						goto Choice2;
					}
					goto Choice1;


				}
				else if (choice == 2)
				{
					goto Loop;
				}

			Choice1:
				if (correct == 1)
				{
					cout << "Tai khoan mat khau da duoc server xac nhan" << endl;
					goto Continue;
				}
				else if (correct == 0)
				{
					cout << "Tai khoan mat khau ban nhap khong dung vui long thu lai" << endl;
					goto Loop;
				}

			Choice2:
				if (correct == 0)
				{
					cout << "Tai khoan da duoc tao chao mung toi server" << endl;
					goto Continue;
				}
				else if (correct == 1)
				{
					cout << "Tai khoan da duoc su dung vui long nhap khac" << endl;
					goto Loop;
				}

			Continue:
				do
				{
					cout << "Nhap 1 de tiep tuc, 0 de thoat: ";
					cin >> continueCheck;
					client.Send(&continueCheck, sizeof(continueCheck), 0);

				} while (continueCheck);
				// Dong ket noi
				client.Close();
			}
			else
			{
				cout << "Khong connect duoc toi server...." << endl;;
			}
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
