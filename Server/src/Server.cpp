// server.cpp: определяет точку входа для консольного приложения.
//
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
#include <map>
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

	#define WSAGetLastError() errno 
	#define closesocket(X) close(X)
	#define SOCKET_ERROR -1
	#define WSACleanup() ;
	#define HOSTENT hostent

	typedef	int SOCKET;
#endif

#define PORT 4179
#define AM_OF_THREADS 50
using namespace std;


struct sock_attr
{
	IN_ADDR* addr;
	thread* t;
	char* get_name()
	{
		return gethostbyaddr((char *)&(addr->s_addr), sizeof(int), AF_INET)->h_name;
	}
	char* get_addr()
	{
		return inet_ntoa(*addr);
	}
};

int last_thread;
map<SOCKET, sock_attr> socks;

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

const char* get_host_name(HOSTENT* hst)
{
    if (!hst)
        return ": ";

    char* cname = hst->h_name;
    int n = strlen(cname);
    char* res = new char[n + 3];
    for (int i = 0; i < n; i++)
        res[i] = cname[i];
    res[n] = ':';
    res[n + 1] = ' ';
    res[n + 2] = 0;
    return res;

}


void new_client(SOCKET my_sock)
{
	char buff[512];
	send(my_sock, "Socket ", sizeof("Socket ") + 1, 0);
	send(my_sock, socks[my_sock].get_name(), strlen(socks[my_sock].get_name()), 0);
	send(my_sock, " connected\n", sizeof(" connected\n") + 1, 0);
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
	{
		buff[bytes_recv] = 0;
		cout << buff;
		for (auto sock : socks)
		{
			if (sock.first != my_sock)
			{
                send(sock.first, socks[my_sock].get_name(), strlen(socks[my_sock].get_name()), 0);
				send(sock.first, &buff[0], bytes_recv, 0);
			}
			else
			{
				int len;
				char* res = to_charp(bytes_recv, len);
                send(sock.first, socks[my_sock].get_name(), strlen(socks[my_sock].get_name()), 0);
				send(sock.first, res, len, 0);
			}
		}
	}
	printf("!Socket %s disconnected\n", socks[my_sock].get_name());
	printf("%d User on-line\n", socks.size());
	closesocket(my_sock);
	socks[my_sock].t->detach();
	socks.erase(my_sock);
	return;
}

int main()
{
	char buff[1024];
	printf("SERVER\n");
	 /*Шаг 1 - Инициализация Библиотеки Сокетов
	 т.к. возвращенная функцией информация не используется,
	 ей передается указатель на рабочий буфер, преобразуемый к указателю
	 на структуру WSADATA.
	 Такой прием позволяет сэкономить одну переменную, однако, буфер
	 должен быть не менее полкилобайта размером (структура WSADATA
	 занимает 400 байт)*/
	last_thread = 0;
#ifdef _WIN32
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		printf("WSAStartup error %d\n", WSAGetLastError());
		return -1;
	}
#endif
	SOCKET mysocket;
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = 0;
	if (bind(mysocket, (const sockaddr *)&local_addr, (int)sizeof(local_addr)))
	{
		printf("Binding error %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}
	if (listen(mysocket, 256))
	{
		printf("Listening error %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}
	printf("Waiting connections...\n");
	SOCKET client_socket;
	sockaddr_in client_addr;
#ifdef _WIN32
    int client_addr_size;
#else
    unsigned int client_addr_size;
#endif
	client_addr_size = sizeof(client_addr);
	while (client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size))
	{
		socks[client_socket].addr = &client_addr.sin_addr;
		printf("New client:\n%s [%s]\n", socks[client_socket].get_name(), socks[client_socket].get_addr());
		printf("%d User on-line\n", socks.size());
        /*last_thread = rand() % AM_OF_THREADS;
        while (last_thread < AM_OF_THREADS && !arr[last_thread]->joinable())
        {
            last_thread++;
        }
		arr[last_thread] = (new thread(new_client, client_socket));
		last_thread++;*/
		socks[client_socket].t = new thread(new_client, client_socket);
	}
	closesocket(mysocket);
	return 0;
}
