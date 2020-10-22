#ifndef UMALLOC_IO_CPP
#define UMALLOC_IO_CPP

#include "include/umalloc/umalloc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <aio.h>

#include <iostream>
#include <chrono>
#include <cstring>
#include <vector>

#define KB (1ul << 10)

namespace bench {

class UmallocIO : public FileIO {
    public:
        void Read(std::string fn , size_t bs,  uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            int fd = open(fn.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
        
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "\nMMap Read\t" << (bs*rcnt/KB)/KB << "MB\t" << time_taken << "s" << std::endl;
            close(fd);
        }
        void Write(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            char *buf = (char *)umalloc(fn.c_str(),(bs*wcnt));
            if (!(buf)) { 
                std::cout<< "Error: "<<strerror(errno)<< "error no = "<< errno<<std::endl;
                exit(2);
            }

             for (off_t i = 0; i < (bs*wcnt); i++)
            {
                buf[i] = 'a';
            }
             // Synchronize with storage to ensure that the latest changes are flushed
            usync(buf);

            // Finally, release the allocation
            ufree(buf);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "\nUMMap Write\t" << (bs*wcnt/KB)/KB << "MB\t" << time_taken << "s" << std::endl;
            
        }
        void AsyncRead(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt ) {}
        void AsyncWrite(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt ) {  }
        void Poll () {}
        void Stat(std::string fn, uint32_t st) {
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
            std::cout<<"File Size (MB) : "<<buf.st_size/KB/KB<<"\n";
            std::cout<<"Number of blocks allocated : "<<buf.st_blocks<<"\n";
            std::cout<<"Time of last access : "<<ctime(&buf.st_atime);
            std::cout<<"Time of last modification : "<<ctime(&buf.st_mtime);
            std::cout<<"Time of last status change : "<<ctime(&buf.st_ctime)<<"\n";
        }
};

} //namespace bench

#endif