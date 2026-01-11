#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include "tcp.cpp"

#define MAX_EVENTS 1024
int nfds = 0;
epoll_event event, event_list[MAX_EVENTS];

void sighandler(int signum)
{
    for (int i = 0; i < nfds; ++i)
    {
        close(event_list[i].data.fd);
    }
    exit(0);
}

int main()
{
    int epoll_fd = epoll_create1(0);

    if (epoll_fd == -1)
    {
        std::cerr << "Failed to create epoll file descriptor." << std::endl;
        return 1;
    }

    signal(SIGINT, sighandler);

    {
        TcpSocket socket(AF_INET, SOCK_STREAM, 0);
        socket.bind(8080);
        socket.listen(SOMAXCONN);
        std::cout << "Listening on port 8080 with fd: " << socket << std::endl;
        event.events = EPOLLIN;
        event.data.fd = socket;
        int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
        if (result == -1)
        {
            std::cerr << "Failed to add socket listener to epoll." << std::endl;
            close(epoll_fd);
            return 1;
        }

        std::array<char, MAX_EVENTS> buff;

        for (;;)
        {
            int nfds = epoll_wait(epoll_fd, event_list, MAX_EVENTS, -1);

            std::cout << "Number of file descriptors with events: " << nfds << std::endl;

            for (int i = 0; i < nfds; ++i)
            {
                if (event_list[i].data.fd == socket)
                {
                    std::cout << "New connection event on socket." << std::endl;
                    int client_fd = socket.accept();
                    if (client_fd == -1)
                    {
                        std::cerr << "Failed to accept connection." << std::endl;
                        continue;
                    }
                    std::cout << "Accepted new connection with fd: " << client_fd << std::endl;
                    event.data.fd = client_fd;
                    int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                    if (result == -1)
                    {
                        std::cerr << "Failed to add client fd to epoll." << std::endl;
                        close(epoll_fd);
                        return 1;
                    }
                }
                else
                {
                    int fd = event_list[i].data.fd;
                    bool received = socket.recv(fd, buff);
                    if (!received)
                    {
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                        close(fd);
                    }
                }
            }
        }
    }
    std::cout << "Listener closed." << std::endl;
    close(epoll_fd);
    return 0;
}
