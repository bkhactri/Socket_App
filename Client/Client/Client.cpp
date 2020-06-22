// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include <conio.h>
// The one and only application object

CWinApp theApp;

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int colWidth = 60;
const int colHeight = 30;

int x3 = 2 * colWidth, y3 = 1, x2 = colWidth, y2 = 1, x = 1, y = 1;

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}

bool checkPos(int x, int y)
{
	if (x == 0 || x == colWidth - 1 || x == 2 * colWidth - 1) return 0;
	if (y == 0 || y == colHeight - 1) return 0;
	return 1;
}

void displayClient()
{
	system("color 0A");
	char c;
	for (size_t i = 0; i < colHeight; i++)
	{
		for (size_t j = 0; j < 2 * colWidth; j++)
		{
			char c;
			if (i == 0)
			{
				if (j == 0) c = 201;
				else if (j == 2 * colWidth - 1) c = 187;
				else if (j == colWidth - 1) c = 203;
				else c = 205;
			}
			else if (i == colHeight - 1)
			{
				if (j == 0) c = 200;
				else if (j == 2 * colWidth - 1) c = 188;
				else if (j == colWidth - 1) c = 202;
				else c = 205;
			}
			else
			{
				if (j == 0 || j == 2 * colWidth - 1 || j == colWidth - 1) c = 186;
				else c = 32;
			}
			cout << c;
		}
		cout << endl;
	}
}

void clearCol(int colNum)
{
	switch (colNum)
	{
	case 1:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = 1; b < colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x = 1; y = 1;
		break;
	case 2:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = colWidth; b < 2 * colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x2 = colWidth; y2 = 1;
		break;
	case 3:
		for (int a = 1; a < colHeight - 1; a++)
		{
			for (int b = 2 * colWidth; b < 3 * colWidth - 1; b++)
			{
				gotoxy(b, a);
				cout << " ";
			}
		}
		x3 = 2 * colWidth; y3 = 1;
		break;
	}
}

void printStringXY(const char* str, int colNum) // colNum la so thu tu cua cot can in
{
	int i = 0;
	switch (colNum)
	{
	case 1:
	{
		for (i; i < strlen(str); i++)
		{
			if (!checkPos(x + 1, y))
			{
				if (y < colHeight - 2) y++;
				else
				{
					clearCol(colNum);
					y = 1;
				}
				x = 1;
			}
			if (str[i] == '\n')
			{
				y++;
				x = 1;
			}
			else
			{
				gotoxy(x, y);
				cout << str[i];
				x++;
			}
		}
		break;
	}
	case 2:
	{
		for (i; i < strlen(str); i++)
		{
			if (!checkPos(x2 + 1, y2))
			{
				if (y2 < colHeight - 2) y2++;
				else
				{
					clearCol(colNum);
					y2 = 1;
				}
				x2 = colWidth;
			}
			if (str[i] == '\n')
			{
				y2++;
				x2 = colWidth;
			}
			else
			{
				gotoxy(x2, y2);
				cout << str[i];
				x2++;
			}
		}
		break;
	}
	}
}

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
	char* UserC = NULL;
	//HMODULE hModule = ::GetModuleHandle(NULL);
	CSocket client;
	AfxSocketInit(NULL);
	client.Create();
	if (client.Connect(CA2W(sAdd), port1))
	{
		do
		{
			client.Receive((char*)&flag, sizeof(int), 0);
			if (flag == 1 || flag == 0)
			{
				printStringXY("\n-- Server da gap loi hoac Server da tat --\n", 1);
				printStringXY("-------- Hen gap lai --------\n", 1);
				client.Close();
				printStringXY("Nhan ENTER de thoat\n", 1);
				Sleep(2200);
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
				string temp;
				temp = temp + UserC + " logged in\n";
				Sleep(40);
				printStringXY(temp.c_str(), 2);
				gotoxy(x, y);
				delete[] UserC;
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
				string temp;
				temp = temp + UserC + " logged out\n";
				Sleep(40);
				printStringXY(temp.c_str(), 2);
				gotoxy(x, y);
				delete[] UserC;
				flag = 0;
			}
		} while (1);

	}

	return 0;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	displayClient();
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
			printStringXY("Nhap dia chi IP cua server: ", 1);
			gets_s(sAdd);
			printStringXY("\n", 1);

			threadStatus = CreateThread(NULL, 0, threadFunction_handle_server_connected, NULL, 0, &threadID);
			Sleep(15);

			if (client.Connect(CA2W(sAdd), port))
			{
				char User[100];
				char Password[100];
				int Usize, Psize;
			Login:
				printStringXY("1.Dang nhap\n", 1);
				printStringXY("2.Tao tai khoan\n", 1);
				printStringXY("Lua chon cua ban: ", 1);
				cin >> choice;
				printStringXY("\n", 1);

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
					printStringXY("Nhap vao tai khoan: ", 1);
					cin.getline(User, 100);
					printStringXY("\n", 1);

					printStringXY("Nhap vao mat khau: ", 1);
					cin.getline(Password, 100);
					printStringXY("\n", 1);

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
					printStringXY("Tai khoan mat khau da duoc server xac nhan\n", 1);
					goto Continue;
				}
				else if (correct == 0)
				{
					printStringXY("Tai khoan mat khau ban nhap khong dung vui long thu lai\n", 1);
					Sleep(2000);
					clearCol(1);
					goto Loop;
				}

			Choice2:
				if (correct == 0)
				{
					printStringXY("Tai khoan da duoc tao thanh cong\n", 1);
					goto Continue;
				}
				else if (correct == 1)
				{
					printStringXY("Tai khoan da duoc su dung vui long nhap khac\n", 1);
					Sleep(2000);
					clearCol(1);
					goto Loop;
				}

			Continue:
				printStringXY("Client da ket noi toi server\n", 1);
				Sleep(2000);
				clearCol(1);

				do
				{

					printStringXY("Moi lua chon tac vu\n", 1);
					printStringXY("1.Upload file len server\n", 1);
					printStringXY("2.Download file tu server\n", 1);
					printStringXY("0.Thoat\n", 1);
					printStringXY("Choice: ", 1);
					cin >> continueCheck;
					printStringXY("\n", 1);

					client.Send(&continueCheck, sizeof(continueCheck), 0);
					if (continueCheck == upload)
					{
						bool isServerOperating;
						client.Receive(&isServerOperating, sizeof(isServerOperating), 0);
						if (isServerOperating == true)
						{
							printStringXY("\nServer dang ban, hay thu hien thao tac sau.\n", 1);
						}
						else
						{
							printStringXY("\nNhap ten file muon upload: ", 1);
							char fileName[100];
							cin.ignore();
							cin.getline(fileName, 100);
							printStringXY("\n", 1);
							int length = strlen(fileName);
							fileName[length] = '\0';
							ifstream f;
							f.open(fileName, ios::in | ios::binary);
							bool exist = false;
							if (f.good())
							{
								exist = true;
								client.Send(&exist, sizeof(exist), 0);
								int s = fileSize(fileName);
								if (s <= max_file_size)
								{
									client.Send(&length, sizeof(length), 0);
									client.Send(fileName, length, 0);
									int buffLength = 0;
									char* buff = new char[1 + 1];
									while (!f.eof()) {
										f.read((char*)buff, 1);
										buffLength = f.gcount();
										cout << "";
										client.Send(&buffLength, sizeof(buffLength), 0);
										client.Send(buff, buffLength, 0);
									}
									delete[] buff;
									buff = NULL;
									string temp;
									temp = temp + "File " + fileName + " da duoc upload len server.\n";
									printStringXY(temp.c_str(), 1);
								}
								else
								{
									string temp;
									temp = temp + "File " + fileName + " khong duoc upload len server do dung luong vuot qua 200MB.\n";
									printStringXY(temp.c_str(), 1);
									string tmp = to_string(double(s) / (pow(1024, 2.0)));
									string temp2;
									temp2 = temp2 + "Dung luong thuc te cua file " + tmp + "MB\n";
									printStringXY(temp2.c_str(), 1);
								}
							}
							else
							{
								client.Send(&exist, sizeof(exist), 0);
								printStringXY("\nKhong ton tai ten file.\n", 1);
							}
							f.close();
						}
					}
					else if (continueCheck == download)
					{
						printStringXY("Danh sach file ton tai tren database cua server:\n", 1);
						char fileName[50];
						int nameLength = 0;
						while (true)
						{
							client.Receive((char*)&nameLength, sizeof(int), 0);
							if (nameLength == 0) break;
							else
							{
								client.Receive((char*)fileName, nameLength, 0);
								fileName[nameLength] = '\0';
								string temp;
								temp = temp + fileName + "\n";
								printStringXY(temp.c_str(), 1);
							}
						}
						printStringXY("\nNhap ten file muon download: ", 1);
						cin.ignore();
						cin.getline(fileName, 100);
						printStringXY("\n", 1);
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
							int buffLength = 0; // do dai doan bin moi lan gui
							char* buff = new char[1 + 1];
							while (true) {
								client.Receive((char*)&buffLength, sizeof(int), 0);
								cout << "";
								if (buffLength == 0) break;
								client.Receive((char*)buff, buffLength, 0);
								buff[buffLength] = '\0';
								output.write(buff, buffLength);
							}
							delete[] buff;
							buff = NULL;
							output.close();
							printStringXY("Da download file thanh cong.\n", 1);
						}
						else
						{
							printStringXY("Khong ton tai file tren server.\n", 1);
						}
					}
					Sleep(2000);
					clearCol(1);
				} while (continueCheck != 0);
				// Dong ket noi
				client.Close();
			}
			else
			{
				printStringXY("Khong connect duoc toi server....\n", 1);
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