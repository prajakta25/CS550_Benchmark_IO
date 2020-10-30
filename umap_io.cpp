#ifndef UMAP_IO_CPP
#define UMAP_IO_CPP

#include "include/umap/umap.h"

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

class UmapIO : public FileIO {
    public:
        void Read(std::string fn , size_t bs,  uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            
            int fd = open(fn.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
        
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "\nUMap Read\t" << (bs*rcnt/KB)<< "KB\t" << wcnt << "\t" << rcnt << "\t" << time_taken << "s" << std::endl;
            close(fd);
        }
        void Write(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt ) {
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            if ( unlink(fn.c_str()) ) {
                int eno = errno;
                if ( eno != ENOENT ) {
                std::cerr << "Failed to unlink " << fn << ": " << strerror(eno) << " Errno=" << eno << std::endl;
                }
            }

            char *wbuffer = (char *)malloc(sizeof(char) * bs);
            int fd = open(fn.c_str(), O_RDWR | O_CREAT | O_TRUNC| O_DIRECT, S_IRWXU | S_IRWXG | S_IRWXO);
            if (fd == -1)
            {
                std::cout << "File cannot be opened\n";
            }
            for (int i = 0; i < bs; i++)
            {   
                wbuffer[i] = 'a';
            }

            // If we are initializing, attempt to pre-allocate disk space for the file.
            try {
                int x;
                if ( ( x = posix_fallocate(fd, 0, (bs*wcnt)) != 0 ) ) {
                int eno = errno;
                std::cerr << "Failed to pre-allocate " << fn << ": " << strerror(eno) << std::endl;
                return;
                }
            } catch(const std::exception& e) {
                std::cerr << "posix_fallocate: " << e.what() << std::endl;
                return;
            } catch(...) {
                int eno = errno;
                std::cerr << "Failed to pre-allocate " << fn << ": " << strerror(eno) << std::endl;
                return;
            }

            std::cout<<"1\n";
            void* base_addr = umap(NULL, (bs*wcnt), PROT_WRITE, UMAP_PRIVATE, fd, 0);
            if ( base_addr == UMAP_FAILED ) {
                int eno = errno;
                std::cerr << "Failed to umap " << fn << ": " << strerror(eno) << std::endl;
                return;
            }
            std::cout<<"2\n";
            std::vector<umap_prefetch_item> pfi;
            char* base = (char*)base_addr;
            uint64_t psize =umapcfg_get_umap_page_size();
            uint64_t PagesInTest = (bs*wcnt) / psize;
            std::cout<<psize<<"\n";
            std::cout << "Prefetching Pages\n";
            for ( int i{0}; i < PagesInTest; ++i) {
                umap_prefetch_item x = { .page_base_addr = &base[i * psize] };
                pfi.push_back(x);
            };
            std::cout<<"3\n";
            umap_prefetch(PagesInTest, &pfi[0]);
            std::cout<<"4\n";
            char *ptr = (char *)base_addr;
            for(int i=0;i<wcnt;i++) {
               memset(ptr, 'a', bs);
            }
           // memset(base, 'a', bs);
            //msync(base, bs, MS_SYNC);


            if (uunmap(base_addr, (bs*wcnt)) < 0) {
                int eno = errno;
                std::cerr << "Failed to uumap " << fn << ": " << strerror(eno) << std::endl;
                return;
            }
            
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            double time_taken = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;
            std::cout << "UMap Write\t" << (bs*wcnt)/KB << "KB\t"<< wcnt << "\t" << rcnt << "\t" << time_taken << "s" << std::endl;
            close(fd);
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
