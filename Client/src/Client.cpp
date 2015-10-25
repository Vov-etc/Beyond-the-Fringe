// consolClient.cpp: определяет точку входа для консольного приложения.
//
//etcp. h
#ifdef _WIN32
#pragma comment ( lib, "ws2_32.lib" )
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <cstdio> 
#include <cstring> 
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
#define PORT 666
#define SERVERADDR "localhost"

typedef u_int SOCKET;

#ifdef __linux__
typedef errno WSAGetLastError();
typedef close() closesocket();
#define SOCKET_ERROR -1
#endif

int main()
{
	char buff[BUFSIZ];
	printf("TCP DEMO CLIENT\n");

	// Шаг 1 - инициализация библиотеки Winsock
#ifdef _WIN32
	WORD wVersion;          // запрашиваемая версия winsock-интерфейса
	WSADATA wsaData;        // сюда записываются данные о сокете
	wVersion = MAKEWORD(2, 0);      // задаем версию winsock

	if (WSAStartup(wVersion, &wsaData))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}
#endif

	// Шаг 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения
	// заполнение структуры sockaddr_in - указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
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

	// адрес сервера получен - пытаемся установить соединение
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Soedinenie s %s uspeshno ustanovlenno\n \
            Type quit for quit\n\n", SERVERADDR);

	// Шаг 4 - чтение и передача сообщений
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки
		buff[nsize] = 0;

		// выводим на экран
		printf("S=>C:%s", buff);

		// читаем пользовательский ввод с клавиатуры
		printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1, stdin);

		// проверка на "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
#ifdef _WIN32 
			WSACleanup();
#endif
			return 0;
		}

		// передаем строку клиента серверу
		send(my_sock, &buff[0], strlen(&buff[0]), 0);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
#ifdef _WIN32 
	WSACleanup();
#endif
	return -1;

	//return 0;
}
