// simple_server.cpp

#include <iostream>
#include <sys/socket.h>   // Socket
#include <arpa/inet.h>    // inet_ntoa
#include <poll.h>         // Poll()
#include <cstring>        
#include <unistd.h>       
#include <errno.h>


#define PORT 12345

int main() {
    // Create TCP Socket
    int tcp_listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (tcp_listen_sockfd < 0) 
    {
        perror("TCP Socket Failed");
        return 1;
    }

    // ReuseAddr settings
    int opt = 1;
    setsockopt(tcp_listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set server addr
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    // TCP Socket bind
    if (bind(tcp_listen_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("TCP Socket Bind failed");
        close(tcp_listen_sockfd);
        return 1;
    }

    // TCP Socket Listen
    if (listen(tcp_listen_sockfd, SOMAXCONN) < 0) 
    {
        perror("TCP Listen Failed");
        close(tcp_listen_sockfd);
        return 1;
    }

    // Create UDP Socket
    int udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd < 0) 
    {
        perror("UDP Socket Failed");
        close(tcp_listen_sockfd);
        return 1;
    }

    // UDP Socket Binding (Doesn't need listen)
    if (bind(udp_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("UDP Socket bind failed");
        close(tcp_listen_sockfd); // close tcp_listen_socket
        close(udp_sockfd);
        return 1;
    }

    std::cout << "Server Started at [Port]: " << PORT << std::endl;

 

    // Set PollFD Structure
    pollfd fds[3];

    // Add Sockets to the poll Structure
    fds[0].fd = tcp_listen_sockfd;
    fds[0].events = POLLIN;

    fds[1].fd = udp_sockfd;
    fds[1].events = POLLIN;

    // Make client Socket FD
    int client_sockfd = -1;
    fds[2].fd = -1;
    fds[2].events = POLLIN;

    // Main loop for blocking APIs
    while (true) 
    {
        int poll_count = poll(fds, 3, -1);
        if (poll_count < 0) 
        {
            perror("poll 실패");
            break;
        }

        // New connection in server tcp listen socket
        if (fds[0].revents & POLLIN) 
        {
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            client_sockfd = accept(tcp_listen_sockfd, (struct sockaddr*)&client_addr, &client_len);
            if (client_sockfd < 0) 
            {
                perror("Accept Failed in fds[0]");
                continue;
            }

            std::cout << "New TCP Client from : " << inet_ntoa(client_addr.sin_addr) << std::endl;

            // Add client_sockfd to PollFD Struct and set to POLLIN
            fds[2].fd = client_sockfd;
            fds[2].events = POLLIN;
        }

        // UDP Socket event detect
        if (fds[1].revents & POLLIN) 
        {
            char buffer[1024];
            sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int bytes_received = recvfrom(udp_sockfd, buffer, sizeof(buffer) - 1, 0,
                                          (struct sockaddr*)&client_addr, &client_len);
            if (bytes_received > 0) 
            {
                buffer[bytes_received] = '\0';
                std::cout << "UDP Client [" << inet_ntoa(client_addr.sin_addr)
                          << ":" << ntohs(client_addr.sin_port) << "] Msg: " << buffer << std::endl;

                sendto(udp_sockfd, buffer, bytes_received, 0,
                       (struct sockaddr*)&client_addr, client_len);
            }
            else 
            {
                perror("UDP data receive failed");
            }
        }

        // TCP Client Socket event detected
        if (fds[2].revents & POLLIN) 
        {
            char buffer[1024];
            int bytes_received = recv(fds[2].fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received > 0) 
            {
                buffer[bytes_received] = '\0';
                std::cout << "TCP Client msg : " << buffer << std::endl;
                send(fds[2].fd, buffer, bytes_received, 0);
            }
            else if (bytes_received == 0)
            {
                std::cout << "TCP Client closed connection" << std::endl;
                close(fds[2].fd);
                fds[2].fd = -1;

            }
            else 
            {
                perror("TCP data receive failed");
                close(fds[2].fd);
                fds[2].fd = -1;

            }
        }
    }

    // Close Socket
    if (client_sockfd >= 0) 
    {
        close(client_sockfd);
    }
    close(tcp_listen_sockfd);
    close(udp_sockfd);

    return 0;
}
