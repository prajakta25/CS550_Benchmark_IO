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
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            //source file
            char* map = (char *)umalloc(fn.c_str(),(bs*wcnt));
            if (!(map)) { 
                std::cout<< "Error: "<<strerror(errno)<< "error no = "<< errno<<std::endl;
                exit(2);
            }

            //destination file
            char* dmap = (char *)umalloc("umalloc_read.txt",(bs*rcnt));
            if (!(dmap)) { 
                std::cout<< "Error: "<<strerror(errno)<< "error no = "<< errno<<std::endl;
                exit(2);
            }

            for(int i = 0; i < rcnt; ++i) {
              memcpy(dmap + (i*bs) , map + (i*bs), bs);
            }

             // Synchronize with storage to ensure that the latest changes are flushed
            usync(map);
            usync(dmap);

            // Finally, release the allocation
            ufree(map);
            ufree(dmap);

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "UMmap_Read\t\t" << (bs*wcnt)/KB /KB<< "m\t\t" << (double)bs/KB/KB <<"m\t\t" << wcnt << "\t\t" << rcnt << "\t\t" << time_taken << "s" << std::endl;
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
        }
        void Write(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt ) {
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            int fd = open(fn.c_str(),  O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (fd == -1)
            {
                std::cout << "File cannot be opened\n";
            }
            // Stretch the file size to the size of the (mmapped) array of ints
            
            int result = lseek(fd, (bs*wcnt), SEEK_SET);
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
            char *buf = (char *)umalloc(fn.c_str(),(bs*wcnt));
            if (!(buf)) { 
                std::cout<< "Error: "<<strerror(errno)<< "error no = "<< errno<<std::endl;
                exit(2);
            }

             for (off_t i = 0; i < wcnt; i++)
            {
                memset(buf + (i*bs) ,'a',bs);
            }
             // Synchronize with storage to ensure that the latest changes are flushed
            usync(buf);

            // Finally, release the allocation
            ufree(buf);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "UMmap_Write\t\t" << (bs*wcnt)/KB /KB<< "m\t\t" << (double)bs/KB/KB <<"m\t\t" << wcnt << "\t\t" << rcnt << "\t\t" << time_taken << "s" << std::endl;
            system("sudo sh -c \"sync && echo 3 > /proc/sys/vm/drop_caches\"");
            
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
            std::cout<<"File Size (KB) : "<<buf.st_size/KB<<"\n";
            std::cout<<"Number of blocks allocated : "<<buf.st_blocks<<"\n";
            std::cout<<"Time of last access : "<<ctime(&buf.st_atime);
            std::cout<<"Time of last modification : "<<ctime(&buf.st_mtime);
            std::cout<<"Time of last status change : "<<ctime(&buf.st_ctime)<<"\n";
        }
};

} //namespace bench

#endif
