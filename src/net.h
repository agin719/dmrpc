#ifndef _NET_H_
#define _NET_H_

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace net
{

struct MsgHeader
{
    int size;
};

class Socket;
using ReadCallback = std::function<void (std::shared_ptr<Socket>, std::string)>;

class Socket
{
public:
    Socket();
    Socket(int fd) : sockfd_(fd) {}
    virtual ~Socket() {}
    
    int connect(const char *ip, int port);
    int listen(int port, ReadCallback callback);

    int fd() { return sockfd_; }
    void close();
    int read(void *buf, int size);
    int write(void *buf, int size);
    int recv(std::string &data);
    int send(std::string &data);

private:
    int sockfd_;
};

class EpollHandle
{
public: 
    EpollHandle(int sockfd) : socket_(std::make_shared<Socket>(sockfd)) {
    }
    virtual ~EpollHandle()
    {
        socket_->close();
    }
    std::shared_ptr<Socket> socket() {
        return socket_;
    }
    void set_read_callback(ReadCallback callback) {
        read_callback_ = callback;
    }
    void read_callback(std::shared_ptr<Socket> sock, std::string data) {
        read_callback_(sock, data);
    }
private:
    std::shared_ptr<Socket>  socket_;
    ReadCallback read_callback_;
};


}

#endif
