//Headers for window
#include <iostream>
#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring> 


const char* SERVER_IP = "192.168.50.149";  // addr of RPi
const int PORT = 12345;

//global sockets and status
SOCKET tcp_sockfd = INVALID_SOCKET;
SOCKET udp_sockfd = INVALID_SOCKET;
bool tcp_connected = false;
bool udp_connected = false;

sockaddr_in server_addr;

void initialize_winsock() 
{
    WSADATA wsaData;
    int init_result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (init_result != 0) 
    {
        std::cerr << "WSAStartup Failed: " << init_result << std::endl;
        exit(1);
    }
}

void  tcp_client_setup() 
{
    // make socket for tcp
    std::cout << "TCP Client Setup Start" << std::endl;

    tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd == INVALID_SOCKET)
    {
        std::cerr << "TCP socket failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }

    // set server addr

    memset(&server_addr, 0, sizeof(server_addr));          // init sockaddr_in with 0
    server_addr.sin_family = AF_INET;                      // IPv4 = AF_INET
    server_addr.sin_port = htons(PORT);                    // port

    // server ip settings
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) 
    {
        std::cerr << "IP convert failed" << std::endl;
        closesocket(tcp_sockfd);
        exit(1);
    }

    // connect
    if (connect(tcp_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        std::cerr << "TCP server connect failed: " << WSAGetLastError() << std::endl;
        closesocket(tcp_sockfd);
        exit(1);
    }

    tcp_connected = true;
    std::cout << "TCP Connected" << std::endl;

}

void udp_client_setup() 
{
    std::cout << "UDP Client Setup Start" << std::endl;
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd == INVALID_SOCKET)
    {
        std::cerr << "UDP Socket failed " << WSAGetLastError() << std::endl;
        exit(1);
    }

    udp_connected = true;
    std::cout << "UDP Connected" << std::endl;
}

void send_msg()
{
    std::string input;
    std::cout << "Input msg: ";
    while(true)
    {
        std::getline(std::cin,input);
        if(strcmp(input.c_str(),"EXIT") == 0)
        {
            std::cout<<"End of Program exiting now.."<<std::endl;
            closesocket(tcp_sockfd);
            closesocket(udp_sockfd);
            break;
        }
        else
        {
            send(tcp_sockfd, input.c_str(), input.length(), 0);
            char buffer[1024];
            int bytes_received = recv(tcp_sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received > 0) 
            {
                buffer[bytes_received] = '\0';
                std::cout << "TCP Server echo: " << buffer << std::endl;
            }

            sendto(udp_sockfd, input.c_str(), input.length(), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            char buffer2[1024];
            sockaddr_in from_addr;
            int from_len = sizeof(from_addr);
            int bytes_received2 = recvfrom(udp_sockfd, buffer2, sizeof(buffer2) - 1, 0, (struct sockaddr*)&from_addr, &from_len);
            if (bytes_received2 > 0) 
            {
                buffer2[bytes_received2] = '\0';
                std::cout << "UDP Server echo: " << buffer2 << std::endl;
            }
        }
    }

}

int main() {
    // init window socket with WSAStartup
    initialize_winsock();
    tcp_client_setup();
    udp_client_setup();

    send_msg();

    //exit
    WSACleanup();

    return 0;
}