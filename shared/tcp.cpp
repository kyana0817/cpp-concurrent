#include <string>
#include <unistd.h>
#include <iostream>
#include <optional>
#include <utility>
#include <memory>
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>

struct TcpSocketHandle
{
    int fd;
    TcpSocketHandle(int fd) : fd(fd) {}
    ~TcpSocketHandle()
    {
        if (fd > 0)
        {
            std::cout << "Closing fd: " << fd << std::endl;
            close(fd);
        }
    }
    TcpSocketHandle(TcpSocketHandle &&other) noexcept : fd(std::exchange(other.fd, -1)) {}
    TcpSocketHandle &operator=(TcpSocketHandle &&other) noexcept
    {
        fd = std::exchange(other.fd, -1);
        return *this;
    }
    TcpSocketHandle(const TcpSocketHandle &) = delete;
    TcpSocketHandle &operator=(const TcpSocketHandle &) = delete;

    int tcpAccept()
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(fd, (sockaddr *)&client_addr, &client_len);
        return client_fd;
    }

    bool tcpReceive(int client_fd, std::array<char, 1024> &buff)
    {
        std::fill(buff.begin(), buff.end(), 0);
        int n = recv(client_fd, buff.data(), buff.size(), 0);

        if (n > 0)
        {
            std::cout << "Received to client: " << std::string(buff.data(), n) << std::endl;
            send(client_fd, buff.data(), n, 0);
            std::cout << "Sent: " << std::string(buff.data(), n) << std::endl;
            return true;
        }
        else if (n == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            close(client_fd);
        }
        else
        {
            std::cerr << "Error receiving data" << std::endl;
            close(client_fd);
        }
        return false;
    }
};

TcpSocketHandle tcpListen(std::string port)
{
    TcpSocketHandle fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(std::stoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd.fd, (sockaddr *)&addr, sizeof(addr));
    listen(fd.fd, SOMAXCONN);

    return std::move(fd);
}
