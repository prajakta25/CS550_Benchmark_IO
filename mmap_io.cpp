#ifndef MMAP_IO_CPP
#define MMAP_IO_CPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <aio.h>

#include <iostream>
#include <chrono>
#include <cstring>

#define KB (1ul << 10)

namespace bench {

class MmapIO : public FileIO {
    public:
        void Read(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            int fd = open(fn.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
            int f = open("mmap_read.txt", O_SYNC | O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            size_t pagesize = getpagesize();
            if (fd == -1)
            {
                std::cout << "file cannot be opened\n";
            }
            struct stat statbuf;
            int err = fstat(fd, &statbuf);
            if(err < 0) {
                std::cout << "file cannot be opened\n";
            }
            char* map = (char*)mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED) 
            {
                std::cout << " Error mmapping the file " << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            // Read the file using mmap
     
            ssize_t n = write(f,map,statbuf.st_size);
            if(n != statbuf.st_size){
                std::cout << "Write failed\n";
            }
            err = munmap(map, statbuf.st_size);

            if(err != 0){
                std::cout<<"UnMapping Failed\n";
                exit(1);
            }
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "\nMMap Read\t" << (p/KB)/KB << "MB\t" << time_taken << "s" << std::endl;
            close(fd);
            close(f);
            
        }
        void Write(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            char *wbuffer = (char *)malloc(sizeof(char) * p);
            int fd = open(fn.c_str(), O_SYNC | O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (fd == -1)
            {
                std::cout << "File cannot be opened\n";
            }
            for (int i = 0; i < p; i++)
            {   
                wbuffer[i] = 'a';
            }
            // Stretch the file size to the size of the (mmapped) array of ints
            
            int result = lseek(fd, p, SEEK_SET);
            if (result == -1)
            {
                std::cout << " Error calling lseek() " << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            result = write(fd, "", 1);
            if (result != 1) 
            {
                std::cout << " Error writing last byte of the file " << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            // The file is ready to be mmapped.
            char *map =(char*) mmap(0, p, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED) 
            {
                std::cout << " Error mmapping the file " << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            //Writing int's to the file as if it were memory (an array of ints).
            memcpy(map, wbuffer, p);
            msync(map, p, MS_SYNC);
            munmap(map, p);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "MMap Write\t" << p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
            close(fd);
        }
        void AsyncRead(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {}
        void AsyncWrite(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {  }
        void Poll () {}
        void Stat(std::string fn, uint32_t st) {}
};

} //namespace bench

#endif
