// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
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


DWORD WINAPI threadFunction_handle_server_connected(LPVOID arg)
{
	unsigned int port1 = 1235;
	int flag = 0;
	int size;
	char* UserC;
	//HMODULE hModule = ::GetModuleHandle(NULL);
	CSocket client;
	AfxSocketInit(NULL);
	client.Create();
	if (client.Connect(CA2W(sAdd), port1))
	{
		do
		{
			fflush(stdin);
			client.Receive((char*)&flag, sizeof(int), 0);
			if (flag == 1 || flag == 0)
			{
				cout << "\n------- Server da gap loi hoac Server da tat -------" << endl;
				cout << "------------------ Hen gap lai ---------------------" << endl;
				client.Close();
				cout << "Nhan ENTER de thoat" << endl;
				exit(0);
			}

			else if (flag == 2)
			{
				//Nhan kich thuoc tai khoan tu client
				client.Receive((char*)&size, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
				UserC = new char[size + 1];
				//Nhan tai khoan
				client.Receive((char*)UserC, size, 0);
				UserC[size] = '\0';
				cout << UserC << " da dang nhap" << endl;
				flag = 0;
			}
			else if (flag == 3)
			{
				//Nhan kich thuoc tai khoan tu client
				client.Receive((char*)&size, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.
				UserC = new char[size + 1];
				//Nhan tai khoan
				client.Receive((char*)UserC, size, 0);
				UserC[size] = '\0';
				cout << UserC << " da dang xuat" << endl;
				flag = 0;
			}
		} while (1);
		cin.clear();
		cin.ignore();
	}
	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{

	int x = 1, y = 1;
	int correct = 0, continueCheck = 0, choice;
	int nRetCode = 0;
	DWORD threadID;
	HANDLE threadStatus;

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
			AfxSocketInit(NULL);

			//Tao socket
			client.Create();

			// Nhap dia chi IP cua server
			cout << "Nhap dia chi IP cua server: ";
			gets_s(sAdd);

			threadStatus = CreateThread(NULL, 0, threadFunction_handle_server_connected, NULL, 0, &threadID);
			Sleep(15);

			if (client.Connect(CA2W(sAdd), port))
			{
				char User[100];
				char Password[100];
				int Usize, Psize;
			Login:
				cout << "1.Dang nhap" << endl;
				cout << "2.Tao tai khoan" << endl;
				cout << "Lua chon cua ban: "; cin >> choice;

				if (cin.fail())
				{
					cin.clear();
					cin.ignore(256, '\n');
					goto Login;
				}

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
					cout << "Tai khoan da duoc tao thanh cong" << endl;
					goto Continue;
				}
				else if (correct == 1)
				{
					cout << "Tai khoan da duoc su dung vui long nhap khac" << endl;
					goto Loop;
				}

			Continue:
				cout << "\nClient da ket noi toi server" << endl;
				do
				{
					cout << "Moi lua chon tac vu" << endl;
					cout << "\n1.Upload file len server\n";
					cout << "2.Download file tu server\n";
					cout << "0.Thoat\n";
					cout << "Choice: ";
					cin >> continueCheck;

					client.Send(&continueCheck, sizeof(continueCheck), 0);
					bool isServerOperating = false;
					client.Receive(&isServerOperating, sizeof(isServerOperating), 0);
					if (isServerOperating == false)
					{
						if (continueCheck == upload)
						{
							cout << "\nNhap ten file muon upload: ";
							char fileName[100];
							cin.ignore();
							cin.getline(fileName, 100);
							int length = strlen(fileName);
							fileName[length] = '\0';
							fstream f;
							f.open(fileName, ios::in | ios::binary);
							bool exist = false;
							if (f.good())
							{
								exist = true;
								client.Send(&exist, sizeof(exist), 0);
								int s = fileSize(fileName);
								if (s <= max_file_size)
								{
									// cout << "Dung luong: " << s << " bytes" << endl;
									client.Send(&length, sizeof(length), 0);
									client.Send(fileName, length, 0);
									int buffLength = 0;
									unsigned char* buff = new unsigned char[max_file_size + 1];
									f.read((char*)buff, max_file_size);
									buffLength = f.gcount();
									buff[buffLength] = '\0';
									client.Send(&buffLength, sizeof(buffLength), 0);
									client.Send(buff, buffLength, 0);
									delete[] buff;

									cout << "File " << fileName << " da duoc upload len server.\n";
								}
								else
								{
									cout << "File " << fileName << " khong duoc upload len server do dung lyong vuot qua 200MB." << endl;
									cout << "Dung luong thuc te cua file " << double(s) / (pow(1024, 2.0)) << "MB" << endl;
								}
							}
							else
							{
								client.Send(&exist, sizeof(exist), 0);
								cout << "\nKhong ton tai ten file.\n";
							}
							f.close();
						}
						else if (continueCheck == download)
						{
							cout << "Danh sach file ton tai tren database cua server:\n";
							char fileName[100];
							int nameLength = 0;
							while (true)
							{
								client.Receive((char*)&nameLength, sizeof(int), 0);
								if (nameLength == 0) break;
								else
								{
									client.Receive((char*)fileName, nameLength, 0);
									fileName[nameLength] = '\0';
									cout << fileName << endl;
								}
							}
							cout << "Nhap ten file muon download: ";
							cin.ignore();
							cin.getline(fileName, 100);
							nameLength = strlen(fileName);
							fileName[nameLength] = '\0';
							client.Send(&nameLength, sizeof(nameLength), 0);
							client.Send(fileName, nameLength, 0);

							bool exist = false;
							client.Receive((char*)&exist, sizeof(bool), 0);
							int buffLength = 0;
							if (exist == true)
							{
								fstream output;
								output.open(fileName, ios::out | ios::binary);
								client.Receive((char*)&buffLength, sizeof(int), 0);
								char* buff = new char[buffLength + 1];
								client.Receive((char*)buff, buffLength, 0);
								buff[buffLength] = '\0';
								output.write(buff, buffLength);
								delete[] buff;
								output.close();
								cout << "Da download file thanh cong.\n";
							}
							else
							{
								cout << "Khong ton tai file tren server.\n";
							}
						}
					}
					else {
						cout << "Server dang ban, hay thu hien thao tac sau.\n";
					}
				} while (continueCheck != 0);
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
	return nRetCode;
}