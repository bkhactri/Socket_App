// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
// The one and only application object

CWinApp theApp;

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char sAdd[1000];

int fileSize(char* path) {
	ifstream f(path, ios::binary);
	f.seekg(0, ios::end);
	int file_size = f.tellg();
	f.seekg(0, ios::beg);
	f.close();
	return file_size;
}


DWORD WINAPI threadFunction_handle_server_connected(LPVOID arg) //Da tieu trinh xu ly nhan biet ket noi server
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
			if (flag == 1 || flag == 0) //co cho biet server da sap
			{
				if (flag == 1)
				{
					printStringXY("\n------------ Server is shut down ---------------------\n", 1);
				}
				else if (flag == 0)
				{
					printStringXY("\n------------ Server ran into the problem ------------\n", 1);
				}
				printStringXY("############## Sorry and See you again ##############\n", 1);
				client.Close();
				printStringXY("Client is exiting securely\n", 1);
				Sleep(2200);
				exit(0);
			}

			else if (flag == 2) //co cho biet thong tin user khac dang nhap
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
			else if (flag == 3) // co cho biet thong tin user khac dang xuat
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
			printStringXY("IP server: ", 1);
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
				printStringXY("1. Log in\n", 1);
				printStringXY("2. Create account\n", 1);
				printStringXY("Your choice: ", 1);
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
					printStringXY("Username: ", 1);
					cin.getline(User, 100);
					printStringXY("\n", 1);

					printStringXY("Password: ", 1);
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
					printStringXY("Your username and password is verified\n", 1);
					goto Continue;
				}
				else if (correct == 0)
				{
					printStringXY("Username or password is not correct\n", 1);
					Sleep(2000);
					clearCol(1);
					goto Loop;
				}

			Choice2:
				if (correct == 0)
				{
					printStringXY("Account has been created successfully\n", 1);
					goto Continue;
				}
				else if (correct == 1)
				{
					printStringXY("Username is already in use. Enter the new one\n", 1);
					Sleep(2000);
					clearCol(1);
					goto Loop;
				}

			Continue:
				printStringXY("Client connects to server\n", 1);
				Sleep(2000);
				clearCol(1);

				do
				{

					printStringXY("Please select task\n", 1);
					printStringXY("1.Upload file to server\n", 1);
					printStringXY("2.Download file from server\n", 1);
					printStringXY("0.Exit\n", 1);
					printStringXY("Your Choice: ", 1);
					cin >> continueCheck;
					printStringXY("\n", 1);

					client.Send(&continueCheck, sizeof(continueCheck), 0);
					if (continueCheck == upload)
					{
						bool isServerOperating;
						client.Receive(&isServerOperating, sizeof(isServerOperating), 0);
						if (isServerOperating == true)
						{
							printStringXY("\nServer is busy. Please try again later\n", 1);
						}
						else
						{
							printStringXY("\nName of the file you want to upload: ", 1);
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
									char* buff = new char[1024 + 1];
									while (!f.eof()) {
										f.read((char*)buff, 1024);
										buffLength = f.gcount();
										cout << "";
										client.Send(&buffLength, sizeof(buffLength), 0);
										client.Send(buff, buffLength, 0);
									}
									buffLength = 0;
									client.Send(&buffLength, sizeof(buffLength), 0);
									delete[] buff;
									buff = NULL;
									string temp;
									temp = temp + "File " + fileName + " is uploaded to server.\n";
									printStringXY(temp.c_str(), 1);
								}
								else
								{
									string temp;
									temp = temp + "File " + fileName + " can't upload to server because size of it is bigger than 200MB.\n";
									printStringXY(temp.c_str(), 1);
									string tmp = to_string(double(s) / (pow(1024, 2.0)));
									string temp2;
									temp2 = temp2 + "Actual size of file: " + tmp + "MB\n";
									printStringXY(temp2.c_str(), 1);
								}
							}
							else
							{
								client.Send(&exist, sizeof(exist), 0);
								printStringXY("\nFile doesn't exist.\n", 1);
							}
							f.close();
						}
					}
					else if (continueCheck == download)
					{
						printStringXY("List file on Database:\n", 1);
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
						printStringXY("\nName of the file you want to download: ", 1);
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
								if (buffLength == 0) break;
								client.Receive((char*)buff, buffLength, 0);
								buff[buffLength] = '\0';
								output.write(buff, buffLength);
							}
							delete[] buff;
							buff = NULL;
							output.close();
							printStringXY("File has been downloaded successfully.\n", 1);
						}
						else
						{
							printStringXY("File doesn't exist on server Database.\n", 1);
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
				printStringXY("Can't connect to the server....\n", 1);
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