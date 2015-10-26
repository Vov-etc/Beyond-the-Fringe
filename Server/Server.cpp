// server.cpp: ���������� ����� ����� ��� ����������� ����������.
//
#ifdef _WIN32
	#pragma comment ( lib, "ws2_32.lib" )
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdio> 
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>
#elif __linux_
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <cerrno>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define WSAGetLastError() errno 
	#define closesocket(X) close(X)
	#define SOCKET_ERROR -1
	#define WSACleanup() ;
	#define HOSTENT hostent

	typedef	int SOCKET;
#endif

#define PORT 4000

using namespace std;

// �������� �������, ������������� �������������� �������������
DWORD WINAPI SexToClient(LPVOID client_socket);
//�������� ������� ������ ����
void changeWords();

char* to_charp(int number, int &len)
{
	string res;
	while (number != 0)
	{
		res.push_back((number % 10) + '0');
		number /= 10;
	}
	if (res.empty())
		res.push_back('0');
	reverse(res.begin(), res.end());
	res.push_back('\n');
	char* ret = new char[res.size() + 1];
	for (int i = 0; i < (int)res.size(); i++)
		ret[i] = res[i];
	ret[res.size()] = 0;
	len = res.size();
	return ret;
}

// ���������� ���������� - ���������� �������� �������������
int nclients = 0;
//���������� ����������, � ������� ��������� �������� �� ������� ������
char *tmp_buff;
char *tmp_buff_obmen = new char;
char *tmp_buff_obmen2 = new char;
set <SOCKET> socks;
//char buff[20 * 1024];
int main()
{
	char buff[1024]; // ����� ��� ��������� ����

	printf("TCP SERVER DEMO\n");
	// ��� 1 - ������������� ���������� �������
	// �.�. ������������ �������� ���������� �� ������������
	// �� ���������� ��������� �� ������� �����, ������������� � ���������
	// �� ��������� WSADATA.
	// ����� ����� ��������� ���������� ���� ����������, ������, �����
	// ������ ���� �� ����� ������������ �������� (��������� WSADATA
	// �������� 400 ����)
#ifdef _WIN32
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());
		return -1;
	}
#endif
	// ��� 2 - �������� ������
	SOCKET mysocket;
	// AF_INET - ����� ���������
	// SOCK_STREAM - ��������� ����� (� ���������� ����������)
	// 0 - �� ��������� ���������� TCP ��������
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// ������!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup(); // �������������� ���������� Winsock
		return -1;
	}

	// ��� 3 - ���������� ������ � ��������� �������
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT); // �� �������� � ������� �������!!!
	local_addr.sin_addr.s_addr = 0; // ������ ��������� �����������
									// �� ��� ���� IP-������

									// �������� bind ��� ����������
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		// ������
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); // ��������� �����!
		WSACleanup();
		return -1;
	}

	// ��� 4 - �������� �����������
	// ������ ������� - 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("ozidanie podkluceniy...\n");

	// ��� 5 - ��������� ��������� �� �������
	SOCKET client_socket; // ����� ��� �������
	sockaddr_in client_addr; // ����� ������� (����������� ��������)
	int client_addr_size = sizeof(client_addr); // ������� accept ���������� �������� ������ ���������

	// ���� ���������� �������� �� ����������� �� �������
	while (client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size))
	{
		nclients++; // ����������� ������� �������������� ��������

					// �������� �������� ��� �����
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, sizeof(int), AF_INET);
		socks.insert(client_socket);

		// ����� �������� � �������
		printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		if (nclients)
		{
			printf("%d User on-line\n", nclients);
		}
		else
		{
			printf("No User on line\n");
		}

		// ����� ������ ������ ��� ����������� �������
		// ��, ��� ����� ������������� ������������ _beginthreadex
		// ��, ��������� ������� ������� ������� ����������� �� ����������
		// ����� �� ������, ����� �������� � CreateThread
		DWORD thID;
		CreateThread(NULL, NULL, SexToClient, &client_socket, NULL, &thID);
	}
	return 0;
}
// ��� ������� ��������� � ��������� ������ 
// � ���������� ���������� ��������������� ������� ���������� �� ��������� 
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	char buff [BUFSIZ]; //char buff[512];
	send(my_sock, "SOCKET PODKLUCHEN\n", sizeof("SOCKET PODKLUCHEN\n") + 1, 0);
	// ���� ���-�������: ����� ������ �� ������� � ����������� �� �������
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
	{
		tmp_buff = buff;
		tmp_buff[bytes_recv] = 0;
		changeWords();
		for (auto sock : socks)
		{
			if (sock != my_sock)
			{
				send(sock, &buff[0], bytes_recv, 0);
			}
			else
			{
				int len;
				char* res = to_charp(bytes_recv, len);
				send(sock, res, len, 0);
			}
		}
	}

	// ���� �� �����, �� ��������� ����� �� ����� �� �������
	// ���������� �������� recv ������ - ���������� � �������� ���������
	nclients--; // ��������� ������� �������� ��������
	printf("-disconnect\n");
	if (nclients)
	{
		printf("%d User on-line\n", nclients);
	}
	else
	{
		printf("No User on line\n");
	}
	// ��������� �����
	closesocket(my_sock);
	return 0;
}
void changeWords()
{
	cout << tmp_buff;
}