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

// прототип функции, обслуживающий подключившихся пользователей
DWORD WINAPI SexToClient(LPVOID client_socket);
//прототип функции обмена слов
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

// глобальная переменная - количество активных пользователей
int nclients = 0;
//глобальная переменная, в которой храняться принятые от сервера данные
char *tmp_buff;
char *tmp_buff_obmen = new char;
char *tmp_buff_obmen2 = new char;
set <SOCKET> socks;
//char buff[20 * 1024];
int main()
{
	char buff[1024]; // Буфер для различных нужд

	printf("TCP SERVER DEMO\n");
	// Шаг 1 - Инициализация Библиотеки Сокетов
	// т.к. возвращенная функцией информация не используется
	// ей передается указатель на рабочий буфер, преобразуемый к указателю
	// на структуру WSADATA.
	// Такой прием позволяет сэкономить одну переменную, однако, буфер
	// должен быть не менее полкилобайта размером (структура WSADATA
	// занимает 400 байт)
#ifdef _WIN32
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		printf("Error WSAStartup %d\n", WSAGetLastError());
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
		printf("Error socket %d\n", WSAGetLastError());
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
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		// Ошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket); // закрываем сокет!
		WSACleanup();
		return -1;
	}

	// Шаг 4 - ожидание подключений
	// размер очереди - 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("ozidanie podkluceniy...\n");

	// Шаг 5 - извлекаем сообщение из очереди
	SOCKET client_socket; // сокет для клиента
	sockaddr_in client_addr; // адрес клиента (заполняется системой)
	int client_addr_size = sizeof(client_addr); // функции accept необходимо передать размер структуры

	// цикл извлечения запросов на подключение из очереди
	while (client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size))
	{
		nclients++; // увеличиваем счетчик подключившихся клиентов

					// пытаемся получить имя хоста
		HOSTENT *hst;
		hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, sizeof(int), AF_INET);
		socks.insert(client_socket);

		// вывод сведений о клиенте
		printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "", inet_ntoa(client_addr.sin_addr));
		if (nclients)
		{
			printf("%d User on-line\n", nclients);
		}
		else
		{
			printf("No User on line\n");
		}

		// Вызов нового потока для обслужвания клиента
		// Да, для этого рекомендуется использовать _beginthreadex
		// но, поскольку никаких вызовов функций стандартной Си библиотеки
		// поток не делает, можно обойтись и CreateThread
		DWORD thID;
		CreateThread(NULL, NULL, SexToClient, &client_socket, NULL, &thID);
	}
	return 0;
}
// Эта функция создается в отдельном потоке 
// и обсуживает очередного подключившегося клиента независимо от остальных 
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	char buff [BUFSIZ]; //char buff[512];
	send(my_sock, "SOCKET PODKLUCHEN\n", sizeof("SOCKET PODKLUCHEN\n") + 1, 0);
	// цикл эхо-сервера: прием строки от клиента и возвращение ее клиенту
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

	// если мы здесь, то произошел выход из цикла по причине
	// возращения функцией recv ошибки - соединение с клиентом разорвано
	nclients--; // уменьшаем счетчик активных клиентов
	printf("-disconnect\n");
	if (nclients)
	{
		printf("%d User on-line\n", nclients);
	}
	else
	{
		printf("No User on line\n");
	}
	// закрываем сокет
	closesocket(my_sock);
	return 0;
}
void changeWords()
{
	cout << tmp_buff;
}