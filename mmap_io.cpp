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
#define FILEPATH "/tmp/mmapped.bin"
#define NUMINTS  (1000)
#define FILESIZE (NUMINTS * sizeof(int))

namespace bench {

class MmapIO : public FileIO {
    public:
        void Read(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            int fd = open(fn.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
            if (fd == -1)
            {
                std::cout << "file cannot be opened";
            }
            int* map = (int*)mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED) 
            {
                std::cout << " Error mmapping the file %s" << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            // Read the file int-by-int from the mmap
     
            for (int i = 1; i <=p; ++i) 
            {
                std::cout<<i<<"\t"<<map[i]<<std::endl;
            }

            if (munmap(map, p) == -1) 
            {
                std::cout << " Error un-mmapping the file" << strerror(errno) << std::endl;
            }
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "MMap Read\t" << (p/KB)/KB << "MB\t" << time_taken << "s" << std::endl;
            close(fd);
            
        }
        void Write(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            int fd = open(fn.c_str(), O_SYNC | O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if (fd == -1)
            {
                std::cout << "file cannot be opened";
            }
            // Stretch the file size to the size of the (mmapped) array of ints
            int result = lseek(fd, FILESIZE-1, SEEK_SET);
            if (result == -1)
            {
                std::cout << " Error calling lseek() %s" << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            result = write(fd, "", 1);
            if (result != 1) 
            {
                std::cout << " Error writing last byte of the file %s" << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            // The file is ready to be mmapped.
            int *map =(int*) mmap(0, p, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED) 
            {
                std::cout << " Error mmapping the file %s" << strerror(errno) << std::endl;
                close(fd);
                exit(2);
            }
            //Writing int's to the file as if it were memory (an array of ints).
            for (int i = 1; i <=p; ++i) 
            {
                map[i] = 2 * i; 
            }
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "MMap Write\t" << p/KB/KB << "MB\t" << time_taken << "s" << std::endl;
            close(fd);
        }
        void AsyncRead(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            
        }
        void AsyncWrite(std::string fn , size_t bs, size_t p, uint32_t wcnt, uint32_t rcnt ) {
            
        }
        void Poll () {

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
            std::cout<<"File Size (MB) : "<<buf.st_size/KB/KB<<"\n";
            std::cout<<"Number of blocks allocated : "<<buf.st_blocks<<"\n";
            std::cout<<"Time of last access : "<<ctime(&buf.st_atime);
            std::cout<<"Time of last modification : "<<ctime(&buf.st_mtime);
            std::cout<<"Time of last status change : "<<ctime(&buf.st_ctime)<<"\n";
    }
};

} //namespace bench

#endif
