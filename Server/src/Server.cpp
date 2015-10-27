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
//#define get_host_name(hst) ((hst) ? hst->h_name : "")
using namespace std;


struct sock_attr
{
    const char* name;
    const char* addr;
    thread* t;
};

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

// Эта функция создается в отдельном потоке 
// и обсуживает очередного подключившегося клиента независимо от остальных 
void new_client(SOCKET client_socket)
{
	SOCKET my_sock = client_socket;
	char buff[BUFSIZ]; //char buff[512];
	send(my_sock, "Socket connected\n", sizeof("Socket connected\n") + 1, 0);
	// цикл эхо-сервера: прием строки от клиента и возвращение ее клиенту
	int bytes_recv;
    sock_attr my_sock_attr = socks[my_sock];
	while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
	{
		buff[bytes_recv] = 0;
		cout << buff;
		for (auto sock : socks)
		{
			if (sock.first != my_sock)
			{
                send(sock.first, my_sock_attr.name, strlen(my_sock_attr.name), 0);
				send(sock.first, &buff[0], bytes_recv, 0);
			}
			else
			{
				int len;
				char* res = to_charp(bytes_recv, len);
                send(sock.first, my_sock_attr.name, strlen(my_sock_attr.name), 0);
				send(sock.first, res, len, 0);
			}
		}
	}

	// если мы здесь, то произошел выход из цикла по причине
	// возращения функцией recv ошибки - соединение с клиентом разорвано
	socks.erase(my_sock);
	printf("!Socket disconnected %d\n", my_sock);
	if (socks.size())
	{
		printf("%d User on-line\n", socks.size());
	}
	else
	{
		printf("No User on-line\n");
	}
	closesocket(my_sock);
	return;
}

int main()
{
	printf("SERVER\n");
	 /*Шаг 1 - Инициализация Библиотеки Сокетов
	 т.к. возвращенная функцией информация не используется
	 ей передается указатель на рабочий буфер, преобразуемый к указателю
	 на структуру WSADATA.
	 Такой прием позволяет сэкономить одну переменную, однако, буфер
	 должен быть не менее полкилобайта размером (структура WSADATA
	 занимает 400 байт)*/
#ifdef _WIN32
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		printf("WSAStartup error %d\n", WSAGetLastError());
		return -1;
	}
#endif
	// Шаг 2 - создание сокета
	SOCKET mysocket;
	// AF_INET - сокет Интернета
	// SOCK_STREAM - потоковый сокет (с установкой соединения)
	// 0 - по умолчанию выбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка!
		printf("Socket error %d\n", WSAGetLastError());
		WSACleanup(); // Деиницилизация библиотеки Winsock
		return -1;
	}
	// Шаг 3 - связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT); // не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0; // сервер принимает подключения
									// на все свои IP-адреса

									// вызываем bind для связывания
	if (bind(mysocket, (const sockaddr *)&local_addr, (int)sizeof(local_addr)))
	{
		printf("Binding error %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	// Шаг 4 - ожидание подключений
	if (listen(mysocket, 256))
	{
		printf("Listening error %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}
	printf("Waiting connections...\n");

	// Шаг 5 - извлекаем сообщение из очереди
	SOCKET client_socket; // сокет для клиента
	sockaddr_in client_addr; // адрес клиента (заполняется системой)
#ifdef _WIN32
    int client_addr_size;
#else
    unsigned int client_addr_size;
#endif
 client_addr_size = sizeof(client_addr); // функции accept необходимо передать размер структуры
	// цикл извлечения запросов на подключение из очереди
	while (client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size))
	{
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, sizeof(int), AF_INET);
        sock_attr new_sock_attr;
        new_sock_attr.name = get_host_name(hst);
        new_sock_attr.addr = inet_ntoa(client_addr.sin_addr);
		printf("New client:\n%s [%s]\n", new_sock_attr.name, new_sock_attr.addr);
        new_sock_attr.t = new thread(new_client, client_socket);
		socks[client_socket] = new_sock_attr;
        printf("%d User on-line\n", socks.size());
	}
    closesocket(mysocket);
	return 0;
}
