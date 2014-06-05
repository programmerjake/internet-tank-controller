#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include "stream.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

inline speed_t translateSerialSpeed(int baud)
{
    switch(baud)
    {
    case 50: return B50;
    case 75: return B75;
    case 110: return B110;
    case 134: return B134;
    case 150: return B150;
    case 200: return B200;
    case 300: return B300;
    case 600: return B600;
    case 1200: return B1200;
    case 1800: return B1800;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default:
        throw logic_error("invalid baud rate");
    }
}

class SerialReader final : public Reader
{
    int fd;
public:
    SerialReader(wstring fileName, int baud = 9600)
    {
        string str = wstringToString(fileName);
        fd = open(str.c_str(), O_RDONLY | O_NOCTTY);
        if(fd == -1)
        {
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        termios tios;
        if(0 != tcgetattr(fd, &tios))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        cfmakeraw(&tios);
        cfsetispeed(&tios, translateSerialSpeed(baud));
        if(0 != tcflush(fd, TCIFLUSH))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        if(0 != tcsetattr(fd, TCSANOW, &tios))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
    }
    virtual ~SerialReader()
    {
        close(fd);
    }
    virtual uint8_t readByte() override
    {
        uint8_t retval;
        static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) == 1 failed");
        ssize_t v = read(fd, (void *)&retval, 1);
        if(v == 1)
            return retval;
        if(v == 0)
            throw EOFException();
        throw IOException(string("IO Error : ") + strerror(errno));
    }
};

class SerialWriter final : public Writer
{
    int fd;
public:
    SerialWriter(wstring fileName, int baud = 9600)
    {
        string str = wstringToString(fileName);
        fd = open(str.c_str(), O_WRONLY | O_NOCTTY);
        if(fd == -1)
        {
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        termios tios;
        if(0 != tcgetattr(fd, &tios))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        cfmakeraw(&tios);
        cfsetospeed(&tios, translateSerialSpeed(baud));
        if(0 != tcflush(fd, TCOFLUSH))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
        if(0 != tcsetattr(fd, TCSANOW, &tios))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
    }
    virtual ~SerialWriter()
    {
        close(fd);
    }
    virtual void writeByte(uint8_t byte) override
    {
        static_assert(sizeof(uint8_t) == 1, "sizeof(uint8_t) == 1 failed");
        ssize_t v = write(fd, (const void *)&byte, 1);
        if(v == 1)
            return;
        int errno_value = ENOSPC;
        if(v == -1)
            errno_value = errno;
        throw IOException(string("IO Error : ") + strerror(errno_value));
    }
    virtual void flush() override
    {
        if(0 != tcdrain(fd))
        {
            close(fd);
            throw IOException(string("IO Error : ") + strerror(errno));
        }
    }
};

#endif // SERIAL_H_INCLUDED
