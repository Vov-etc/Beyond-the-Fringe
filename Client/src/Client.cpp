// consolClient.cpp: определяет точку входа для консольного приложения.
//
//etcp. h
#ifdef _WIN32
	#pragma comment ( lib, "ws2_32.lib" )
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <iostream>
#include <set>
#include <algorithm>
#include <cstdio>
#include <thread>
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>
    #define SYSTEM_PAUSE system("pause")
#else
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
	#define SYSTEM_PAUSE system("wait")

	typedef	int SOCKET;
#endif


using namespace std;


#define PORT 4179
#define SERVERADDR "127.0.0.1"


thread* in, *out;


void in_f(SOCKET my_sock)
{
	char buff[BUFSIZ];
	while (true)
	{
		fgets(buff, sizeof(buff) - 1, stdin);
		if (!strcmp(buff, "quit\n"))
		{
			printf("Exit...");
			in->detach();
			out->detach();
			return;
		}
		send(my_sock, buff, strlen(buff), 0);
	}
}


void out_f(SOCKET my_sock)
{
	char buff[BUFSIZ];
	int nsize;
	while ((nsize = recv(my_sock, buff, sizeof(buff) - 1, 0)) > 0)
	{
		buff[nsize] = 0;
		printf("S=>C:\n-----\n%s\n-----\n", buff);
	}
	printf("Recieve error %d\n", WSAGetLastError());
	in->detach();
	out->detach();
	return;
}


int main()
{
	printf("CLIENT\n");
#ifdef _WIN32
	WORD wVersion = MAKEWORD(2, 0);     // запрашиваемая версия winsock-интерфейса
	WSADATA wsaData;					// сюда записываются данные о сокете

	if (WSAStartup(wVersion, &wsaData))
	{
		printf("WSAStartup error %d\n", WSAGetLastError());
		return -1;
	}
#endif
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket error %d\n", WSAGetLastError());
		return -1;
	}
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	if (inet_addr(SERVERADDR) != INADDR_NONE)
	{
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	}
	else
	{
		if (gethostbyname(SERVERADDR)) 
        {
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
            dest_addr.sin_addr.s_addr = inet_addr(gethostbyname(SERVERADDR)->h_addr_list[0]);
        }
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}
	printf("Connection with %s is successfull\nType quit for quit\n\n", SERVERADDR);
	in = new thread(in_f, my_sock);
	out = new thread(out_f, my_sock);
	while (in->joinable() && out->joinable());
	closesocket(my_sock);
	WSACleanup();
	return 0;
}
