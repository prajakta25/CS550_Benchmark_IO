#ifndef POSIX_IO_CPP
#define POSIX_IO_CPP

#include "file_io.cpp"

#include <sys/types.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <chrono>
#include <cstring>

#define KB (1ul << 10)

namespace bench
{

class PosixIO : public FileIO
{
public:
    void Read(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt )
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        char *rbuffer = (char *)malloc(sizeof(char) * bs);
        int fd = open(fn.c_str(), O_SYNC | O_RDONLY, 0);
        if (fd == -1)
        {
            std::cout << "file cannot be opened";
        }
        while (read(fd, rbuffer, bs) != 0)
            ;
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
        std::cout << "Read\t" << p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
        close(fd);
        free(rbuffer);
    }
    void Write(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt )
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        char *wbuffer = (char *)malloc(sizeof(char) * bs);
        int f = open(fn.c_str(), O_SYNC | O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        if (f == -1)
        {
            std::cout << "file cannot be opened";
        }
        int len = p / bs;

        for (int i = 0; i < bs; i++)
        {
            wbuffer[i] = 'a';
        }
        for (int i = 0; i < len; i++)
        {
            int n=0;
            if ((n=write(f, wbuffer, bs)) == -1)
            {  
                std::cout << " Error at write(): %s" << strerror(errno) << std::endl;
                close(f);
                exit(2);
            }
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
        std::cout << "Write\t" << p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
        close(f);
        free(wbuffer);
    }
    void AsyncRead(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt)
    {
        aiocb cb;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        int rec_size = p;
        char *rbuffer = (char *)malloc(sizeof(char) * rec_size);
        memset(&cb, 0, sizeof(struct aiocb));
        cb.aio_buf = rbuffer;
        cb.aio_nbytes = p;
        int file = open(fn.c_str(), O_RDONLY, 0);

        if (file == -1)
        {
            std::cout << "Unable to open file!" << std::endl;
        }
        cb.aio_fildes = file;
        // read!
        if (aio_read(&cb) == -1)
        {
            std::cout << "Unable to create request!" << std::endl;
            close(file);
        }

        // wait until the request has finished
        while (aio_error(&cb) == EINPROGRESS)
            ;

        // success
        int numBytes = aio_return(&cb);

        if (numBytes != -1)
        {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "AIO Read\t" <<  p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
        }
        else
            std::cout << "Error!" << std::endl;
        close(file);
        free(rbuffer);
    }
    void AsyncWrite(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt )
    {
        struct aiocb cb;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        int rec_size = p;
        char *wbuffer = (char *)malloc(sizeof(char) * rec_size);
        int fd = open(fn.c_str(), O_SYNC | O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        if (fd == -1)
        {
            std::cout << "file cannot be opened";
        }
        //fill the buffer with  data
        memset(&cb, 0, sizeof(struct aiocb));
        cb.aio_nbytes=p;
        cb.aio_fildes = fd;

        for (int i = 0; i < rec_size; i++)
        {
            wbuffer[i] = 'a';
        }
        cb.aio_buf = wbuffer;
        if (aio_write(&cb) == -1)
        {
            std::cout << " Error at aio_write(): %s" << strerror(errno) << std::endl;
            close(fd);
            exit(2);
        }

        /* Wait until completion */
        while (aio_error(&cb) == EINPROGRESS)
            ;

        int err = aio_error(&cb);
        int ret = aio_return(&cb);

        if (err != 0)
        {
            std::cout << " Error at aio_error() : %s\n"
                 << strerror(err);
            close(fd);
            exit(2);
        }

        if (ret != cb.aio_nbytes)
        {
            std::cout << " Error at aio_return()" << std::endl;
            close(fd);
            exit(2);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
        std::cout << "AIO Write\t" << p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
        close(fd);
        free(wbuffer);
    }
    void Poll () {
         std::cout<<"";
    }
    void Stat(){
        std::cout<<"";
    }
};

} // namespace bench

#endif