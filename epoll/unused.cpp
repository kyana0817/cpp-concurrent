#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include "tcp.cpp"

int main()
{
    int epoll_fd = epoll_create1(0);

    {
        TcpSocket socket(AF_INET, SOCK_STREAM, 0);
        socket.bind(8080);
        socket.listen(SOMAXCONN);
        std::cout << "Listening on port 8080 with fd: " << socket << std::endl;

        for (;;)
        {
            int client_fd = socket.accept();
            if (client_fd == -1)
            {
                std::cerr << "Failed to accept connection." << std::endl;
                continue;
            }
            std::cout << "Accepted new connection with fd: " << client_fd << std::endl;

            std::array<char, 1024> buff;

            while (socket.recv(client_fd, buff))
                ;
        }
    }
    std::cout << "Listener closed." << std::endl;
    return 0;
}
