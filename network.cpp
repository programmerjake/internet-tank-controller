#include "network.h"
#include "util.h"
#ifdef _WIN32 // Windows
#error finish
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <netinet/tcp.h>

using namespace std;

namespace
{
initializer signalInit([]()
{
    signal(SIGPIPE, SIG_IGN);
});

class NetworkWriter final : public Writer
{
private:
    vector<uint8_t> buffer;
    int fd;
public:
    NetworkWriter(int fd)
        : fd(fd)
    {
        int flag = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(flag));
    }
    virtual ~NetworkWriter()
    {
        close(fd);
    }
    virtual void writeByte(uint8_t v)
    {
        buffer.push_back(v);
        if(buffer.size() >= 16384)
            flush();
    }
    virtual void flush()
    {
        const uint8_t * pbuffer = buffer.data();
        ssize_t sizeLeft = buffer.size();
        while(sizeLeft > 0)
        {
            ssize_t retval = send(fd, (const void *)pbuffer, sizeLeft, 0);
            if(retval == -1)
            {
                throw IOException(string("io error : ") + strerror(errno));
            }
            else
            {
                sizeLeft -= retval;
                pbuffer += retval;
            }
        }
        int flag = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(flag));
        buffer.clear();
    }
};
}

NetworkConnection::NetworkConnection(wstring url, uint16_t port)
{
    string url_utf8 = wstringToString(url), port_str = to_string((unsigned)port);
    addrinfo *addrList = nullptr;
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_family = 0;
    int retval = getaddrinfo(url_utf8.c_str(), port_str.c_str(), &hints, &addrList);

    if(0 != retval)
    {
        throw NetworkException(string("getaddrinfo: ") + gai_strerror(retval));
    }

    int fd = -1;

    for(addrinfo *addr = addrList; addr; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if(fd == -1)
        {
            continue;
        }
        if(-1 != connect(fd, addr->ai_addr, addr->ai_addrlen))
        {
            break;
        }

        close(fd);
        fd = -1;
    }

    if(fd == -1)
    {
        string msg = "can't connect: ";
        msg += strerror(errno);
        freeaddrinfo(addrList);
        throw NetworkException(msg);
    }

    int flag = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(flag));

    freeaddrinfo(addrList);
    readerInternal = unique_ptr<Reader>(new FileReader(fdopen(dup(fd), "r")));
    writerInternal = unique_ptr<Writer>(new NetworkWriter(fd));
}

NetworkServer::NetworkServer(uint16_t port)
{
    addrinfo hints;
    memset((void *)&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    addrinfo *addrList;
    string port_str = to_string((unsigned)port);
    int retval = getaddrinfo(NULL, port_str.c_str(), &hints, &addrList);

    if(0 != retval)
    {
        throw NetworkException(string("getaddrinfo: ") + gai_strerror(retval));
    }

    fd = -1;
    const char *errorStr = "getaddrinfo";

    for(addrinfo *addr = addrList; addr != NULL; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        errorStr = "socket";

        if(fd < 0)
        {
            continue;
        }

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

        if(::bind(fd, addr->ai_addr, addr->ai_addrlen) == 0)
        {
            break;
        }

        int temp = errno;
        close(fd);
        errno = temp;
        errorStr = "bind";
        fd = -1;
    }

    if(fd < 0)
    {
        string msg = string(errorStr) + ": ";
        msg += strerror(errno);
        freeaddrinfo(addrList);
        throw NetworkException(msg);
    }

    freeaddrinfo(addrList);

    if(listen(fd, 50) == -1)
    {
        string msg = "listen: ";
        msg += strerror(errno);
        close(fd);
        throw NetworkException(msg);
    }
}

NetworkServer::~NetworkServer()
{
    close(fd);
}

shared_ptr<StreamRW> NetworkServer::accept()
{
    int fd2 = ::accept(fd, nullptr, nullptr);

    if(fd2 < 0)
    {
        string msg = "accept: ";
        msg += strerror(errno);
        throw NetworkException(msg);
    }

    int flag = 1;
    setsockopt(fd2, IPPROTO_TCP, TCP_NODELAY, (const void *)&flag, sizeof(flag));

    shared_ptr<Reader> reader = shared_ptr<Reader>(new FileReader(fdopen(dup(fd2), "r")));
    shared_ptr<Writer> writer = shared_ptr<Writer>(new NetworkWriter(fd2));
    return shared_ptr<StreamRW>(new StreamRWWrapper(reader, writer));
}
#endif
