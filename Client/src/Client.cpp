// consolClient.cpp: ���������� ����� ����� ��� ����������� ����������.
//
//etcp. h
#ifdef _WIN32
#pragma comment ( lib, "ws2_32.lib" )
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <cstdio> 
#include <cstring> 
#include <cstdlib>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#define PORT 4000
#define SERVERADDR "127.0.0.1"

typedef int SOCKET;

#ifdef __linux__
#define WSAGetLastError() errno 
#define closesocket(X) close(X)
#define SOCKET_ERROR -1
#define HOSTENT hostent
#endif


int main()
{
	char buff[BUFSIZ];
	printf("TCP DEMO CLIENT\n");

	// ��� 1 - ������������� ���������� Winsock
#ifdef _WIN32
	WORD wVersion;          // ������������� ������ winsock-����������
	WSADATA wsaData;        // ���� ������������ ������ � ������
	wVersion = MAKEWORD(2, 0);      // ������ ������ winsock

	if (WSAStartup(wVersion, &wsaData))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}
#endif

	// ��� 2 - �������� ������
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// ��� 3 - ��������� ����������
	// ���������� ��������� sockaddr_in - �������� ������ � ����� �������
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	// �������������� IP ������ �� ����������� � ������� ������
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// ������� �������� IP ����� �� ��������� ����� �������
		if (gethostbyname(SERVERADDR)) 
        {
			/// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
            dest_addr.sin_addr.s_addr = inet_addr(gethostbyname(SERVERADDR)->h_addr_list[0]);
        }
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
#ifdef _WIN32
			WSACleanup();
#endif
			return -1;
		}
	}

	// ����� ������� ������� - �������� ���������� ����������
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Soedinenie s %s uspeshno ustanovlenno\n \
            Type quit for quit\n\n", SERVERADDR);

	// ��� 4 - ������ � �������� ���������
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ������ ����������� ���� � ����� ������
		buff[nsize] = 0;

		// ������� �� �����
		printf("S=>C:%s", buff);

		// ������ ���������������� ���� � ����������
		printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1, stdin);

		// �������� �� "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// ���������� �����
			printf("Exit...");
			closesocket(my_sock);
#ifdef _WIN32 
			WSACleanup();
#endif
			return 0;
		}

		// �������� ������ ������� �������
		send(my_sock, &buff[0], strlen(&buff[0]), 0);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
#ifdef _WIN32 
	WSACleanup();
#endif
	//return -1;

	return 0;
}
