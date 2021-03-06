#ifndef POSIX_IO_CPP
#define POSIX_IO_CPP

#include "file_io.cpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include<time.h>

#include <iostream>
#include <chrono>
#include <cstring>

#define KB (1ul << 10)

namespace bench
{

class PosixIO : public FileIO
{
public:
    void Read(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt )
    {
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
        double sum=0.00;
        char *rbuffer = (char *)malloc(sizeof(char) * bs);
        int fd = open(fn.c_str(),  O_RDONLY, 0);
        if (fd == -1)
        {
            std::cout << "File cannot be opened\n";
        }
    
        for(int i=0; i<rcnt; i++) {
            std::chrono::steady_clock::time_point b = std::chrono::steady_clock::now();
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
            read(fd, rbuffer, bs);
            std::chrono::steady_clock::time_point e = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(e - b).count()) / 1000000.0;
            sum+=time_taken;
            std::cout << "Read\t\t" << (bs*wcnt)/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t"<< wcnt << "\t\t" << i << "\t\t" << time_taken << "s" << std::endl;
        }
        std::cout << "Total_Read\t\t" << (bs*rcnt)/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t"<< wcnt << "\t\t" << rcnt << "\t\t" << (sum/rcnt) << "s" << std::endl;
        close(fd);
        free(rbuffer);
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
    }
    void Write(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt )
    {
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
        double sum=0.00;
        char *wbuffer = (char *)malloc(sizeof(char) * bs);
        int f = open(fn.c_str(),  O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        if (f == -1)
        {
            std::cout << "File cannot be opened\n";
        }

        for (int i = 0; i < bs; i++)
        {
            wbuffer[i] = 'a';
        }
        for (int i = 0; i < wcnt; i++)
        {
            std::chrono::steady_clock::time_point b = std::chrono::steady_clock::now();
            int n=0;
            if ((n=write(f, wbuffer, bs)) == -1)
            {  
                //std::cout << " Error at write(): " << strerror(errno) << std::endl;
                close(f);
                //exit(2);
            }
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
             std::chrono::steady_clock::time_point e = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(e - b).count()) / 1000000.0;
            sum+=time_taken;
            std::cout << "Write\t\t" << (bs*wcnt)/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t"<< i << "\t\t" << rcnt << "\t\t" << time_taken << "s" << std::endl;

        }
        std::cout << "Total_Write\t\t" << (bs*wcnt)/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t" << wcnt << "\t\t" << rcnt << "\t\t" << (sum/wcnt) << "s" << std::endl;
        close(f);
        free(wbuffer);
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
    }
    void AsyncRead(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt)
    {
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
        aiocb cb;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        char *rbuffer = (char *)malloc(sizeof(char) * bs*rcnt);
        memset(&cb, 0, sizeof(struct aiocb));
        cb.aio_buf = rbuffer;
        cb.aio_nbytes = bs*rcnt;
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
            std::cout << "AIO_Read\t\t" << (bs*wcnt)/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t" << wcnt << "\t\t" << rcnt << "\t\t" << time_taken << "s" << std::endl;
        }
        else 
            std::cout << "Error!" << std::endl;
        close(file);
        free(rbuffer);
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
    }
    void AsyncWrite(std::string fn , size_t bs,  uint32_t wcnt, uint32_t rcnt )
    {
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
        struct aiocb cb;
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        int rec_size = bs*wcnt;
        char *wbuffer = (char *)malloc(sizeof(char) * rec_size);
        int fd = open(fn.c_str(),  O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        if (fd == -1)
        {
            std::cout << "File cannot be opened\n";
        }
        //fill the buffer with  data
        memset(&cb, 0, sizeof(struct aiocb));
        cb.aio_nbytes=rec_size;
        cb.aio_fildes = fd;

        for (int i = 0; i < rec_size; i++)
        {
            wbuffer[i] = 'a';
        }
        cb.aio_buf = wbuffer;
        if (aio_write(&cb) == -1)
        {
            std::cout << " Error at aio_write(): " << strerror(errno) << std::endl;
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
            std::cout << " Error at aio_error() : \n"
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
        std::cout << "AIO_Write\t\t" << rec_size/KB/KB << "m\t\t" << (double)bs/KB/KB <<"m\t\t" << wcnt << "\t\t" << rcnt << "\t\t" << time_taken << "s" << std::endl;
        close(fd);
        free(wbuffer);
        system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
    }
    void Poll () {
         std::cout<<"";
    }
    void Stat(std::string fn, uint32_t st){
        struct stat buf;
        stat(fn.c_str(),&buf);
        std::cout<<"\nStats:"<<"\n";
        std::cout<<"ID of device containing file :"<<buf.st_dev<<"\n";
        std::cout<<"Inode number : "<<buf.st_ino<<"\n";
        std::cout<<"File Access :";
        if(buf.st_mode & R_OK) {
            std::cout<<"Read ";
        } 
        if(buf.st_mode & W_OK) {
            std::cout<<"Write ";
        }
        if(buf.st_mode & X_OK) {
            std::cout<<"Execute ";
        }
        std::cout<<"\n";
        std::cout<<"Number of hard links : "<<buf.st_nlink<<"\n";
        std::cout<<"User ID of owner : "<<buf.st_uid<<"\n";
        std::cout<<"Group ID of owner : "<<buf.st_gid<<"\n";
        std::cout<<"File Size (KB) : "<<buf.st_size/KB/KB<<"\n";
        std::cout<<"Number of blocks allocated : "<<buf.st_blocks<<"\n";
        std::cout<<"Time of last access : "<<ctime(&buf.st_atime);
        std::cout<<"Time of last modification : "<<ctime(&buf.st_mtime);
        std::cout<<"Time of last status change : "<<ctime(&buf.st_ctime)<<"\n";
    }
};

} // namespace bench

#endif