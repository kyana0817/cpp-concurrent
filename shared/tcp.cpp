#include <string>
#include <unistd.h>
#include <iostream>
#include <optional>
#include <utility>
#include <memory>
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>

class TcpSocket
{
private:
    int m_fd;
    int m_family;

public:
    TcpSocket(int domain, int type, int protocol)
    {
        m_family = domain;
        m_fd = socket(domain, type, protocol);
        int opt = 1;
        if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        {
            std::cerr << "Failed to set SO_REUSEADDR option" << std::endl;
        }
    }
    ~TcpSocket() noexcept
    {
        if (m_fd > 0)
        {
            std::cout << "Closing fd: " << m_fd << std::endl;
            close(m_fd);
        }
    }
    operator int() const
    {
        return m_fd;
    }
    void bind(int port)
    {
        sockaddr_in addr;
        addr.sin_family = m_family;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
        if (::bind(m_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
        {
            std::cerr << "Failed to bind socket to port " << port << std::endl;
        }
    }
    void listen(int queue_length) noexcept
    {
        if (::listen(m_fd, queue_length) == -1)
        {
            std::cerr << "Failed to listen on socket" << std::endl;
        }
    }
    int accept()
    {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        return ::accept(m_fd, (sockaddr *)&client_addr, &client_len);
    }
    bool recv(int client_fd, std::array<char, 1024> &buff)
    {
        std::fill(buff.begin(), buff.end(), 0);
        int n = ::recv(client_fd, buff.data(), buff.size(), 0);

        if (n > 0)
        {
            std::cout << "Received to client: " << std::string(buff.data(), n) << std::endl;
            ::send(client_fd, buff.data(), n, 0);
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
