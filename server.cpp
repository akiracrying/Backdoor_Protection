#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> // их слишком много 2/3 не нужны вообще
#include <time.h>
#include <stdbool.h>
#include <io.h>

//#include <sys/socket.h>
#include <winsock2.h> // для структур WSDATA и др.
#include <windows.h>
#pragma comment(lib, "ws2_32.lib") // также
#define PORT 8877

int main() {

	WSADATA ws;//стркутура с инфой о сокетах
	WSAStartup(MAKEWORD(2, 2), &ws); //подключаем dll 

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0); //объявляем сокет с ipv4 и TСP
	SOCKADDR_IN sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET; // создаём структуру и указываем ipv4 
		sa.sin_port = htons(PORT); // меняет представление порта на понятный для системы
	printf("you are server\n");
	//сервер
		bind(server, (struct sockaddr*)&sa, sizeof(sa)); // биндим сервак
		listen(server, 100); // это сколько и откуда файлов мы берём
		SOCKET client_socket;
		SOCKADDR_IN client_addr;
		int client_addr_size = sizeof(client_addr);

		while (client_socket = accept(server, (struct sockaddr*)&client_addr, &client_addr_size)) {
			printf("connect OK\n");
			char file_name[50]; // указываем имя файла который нужно аннигилировать 
			printf("enter file name\n");
			scanf("%s", &file_name);
			send(client_socket, file_name, sizeof(file_name), 0); // отсылаем 
			char code;
			recv(client_socket, &code, sizeof(code), 0);
			if (code == '1')printf("error\n");
			else printf("good");
			//printf("%c", code[0]);
		}

	//setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, a, sizeof(opt));
	return 0;
}