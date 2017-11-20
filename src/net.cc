#include "net.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>
#include <glog/logging.h>
#define MAX_EVENTS 1000

using namespace std;

namespace net
{

static int epoll_op(int epfd, int op, int fd, unsigned int events, EpollHandle *handle)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = handle;
    if (epoll_ctl(epfd, op, fd, &event) < 0)
    {
        LOG(ERROR) << " epoll_ctl error ";
        return -1;
    }
    return 0;
}

Socket::Socket() :
    sockfd_(::socket(AF_INET, SOCK_STREAM, 0))
{
}

int Socket::connect(const char *ip, int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    
    if (::connect(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        LOG(FATAL) << "Socket::connect connect failed";
        return -1;
    }
    return 0;
}

int Socket::listen(int port, ReadCallback callback)
{
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    if (bind(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        LOG(FATAL) << "Socket::listen bind error";
        return -1;
    }
    ::listen(sockfd_, 1);

    int epfd = epoll_create(1);
    EpollHandle *handle = new EpollHandle(sockfd_);
    handle->set_read_callback(callback);
    epoll_op(epfd, EPOLL_CTL_ADD, sockfd_, EPOLLIN|EPOLLRDHUP|EPOLLERR, handle);

    struct epoll_event events[MAX_EVENTS];
    for ( ; ; )
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++)
        {
            EpollHandle *handle = reinterpret_cast<EpollHandle*>(events[i].data.ptr);
            shared_ptr<Socket> sock = handle->socket();

            if (events[i].events&EPOLLERR || events[i].events&EPOLLRDHUP)
            {
                epoll_op(epfd, EPOLL_CTL_DEL, sock->fd(), events[i].events, NULL);
                delete handle;
            } else if (sock->fd() == sockfd_)
            {
                int sockfd = accept(sockfd_, NULL, 0);
                EpollHandle *handle = new EpollHandle(sockfd);
                handle->set_read_callback(callback);
                epoll_op(epfd, EPOLL_CTL_ADD, sockfd, EPOLLIN|EPOLLERR|EPOLLRDHUP, handle);
            } else if (events[i].events & EPOLLIN)
            {
                string data;
                sock->recv(data);
                handle->read_callback(sock, data);
            }
        }
    }
}

void Socket::close()
{
    ::close(sockfd_);
}

int Socket::read(void *buf, int size)
{
    char *tbuf = reinterpret_cast<char*>(buf);
    int nbytes = 0;
    do 
    {
        int len = ::read(sockfd_, tbuf+nbytes, size-nbytes);
        if (len < 0)
        {
            if (errno == EINTR)
                continue;
            LOG(FATAL) << "Socket::read read error: " << strerror(errno); 
            return -1;
        } else if (len == 0) return 0;
        nbytes += len;
    } while (nbytes < size);
    return nbytes;
}

int Socket::write(void *buf, int size)
{
    char *tbuf = reinterpret_cast<char*>(buf);
    int nbytes = 0;
    do 
    {
        int len = ::write(sockfd_, tbuf+nbytes, size-nbytes);
        if (len < 0)
        {
            if (errno == EINTR)
                continue;
            LOG(FATAL) << "Socket::read read error"; 
            return -1;
        } else if (len == 0) return 0;
        nbytes += len;
    } while (nbytes < size);
    return nbytes;
}

int Socket::recv(string &data)
{
    MsgHeader header;
    int rt = read((void*)&header, sizeof(header));
    if (rt < 0) return -1;
    char buf[header.size];
    rt = read((void*)buf, header.size);
    if (rt < 0) return -1;
    data = std::move(string(buf, header.size));

    return 0;
}

int Socket::send(string &data)
{
    MsgHeader header;
    header.size = data.size();
    int rt = write((void*)&header, sizeof(header));
    if (rt < 0) return -1;
    rt = write((void*)data.c_str(), data.size());
    if (rt < 0) return -1;
    return 0;
}

}

