#include "file_io_factory.cpp"

#include <iostream>
#include <memory>

#include "arguments.h"

#define KB (1ul << 10)

class ArgsParser : public common::args::ArgMap
{
private:
    /*
        This function defines required arguments.
        This is where you assert certain arguments are set.
    */

    void VerifyArgs(void)
    {
        AssertOptIsSet("-client");
        AssertOptIsSet("-fn");
    }

public:
    /*
        This function simply prints the usage of the program
    */

    void Usage(void)
    {
        std::cout << "Usage: ./generator -[param] value ...";
        std::cout << std::endl;

        std::cout << "-client [string]: Which I/O method to use. Posix or MMap or UMap." << std::endl;
        std::cout << "-fn [string]: The filename to perform I/O with" << std::endl;
        std::cout << "-bs [size]: The block size for I/O requests. Each Read()/Write() will write this amount of data" << std::endl;
        std::cout << "-rcnt [int]: The number of read requests to generate" << std::endl;
        std::cout << "-wcnt [int]: The number of write requests to generate" << std::endl;
        std::cout << "-stat [int]: The number of stat requests to generate." << std::endl;
        std::cout << "-h: Print this help message" << std::endl;

        std::cout << std::endl;
        std::cout << "Format of size: [integer][bBkKmMgG]. Ex: 4k and 4K, 4g and 4G, etc." << std::endl;
    }

    /*
        This function actually parses the argument vector
    */

    ArgsParser(int argc, char **argv)
    {
        /*
            A StringMap converts a string to an enumeration.
            In this case, the enumeration is of type bench::FileIOType
            This is a required argument and thus does not have a default value.
        */
        AddOpt("-client", common::args::ArgType::kStringMap);
        AddStringMapVal("-client", "Posix", static_cast<int>(bench::FileIOType::kPosixIO));
        AddStringMapVal("-client", "PosixAsync", static_cast<int>(bench::FileIOType::kPosixIO));
        AddStringMapVal("-client", "Mmap", static_cast<int>(bench::FileIOType::kmmapIO));
        AddStringMapVal("-client", "Umap", static_cast<int>(bench::FileIOType::kUmapIO));
        AddStringMapVal("-client", "Ummap", static_cast<int>(bench::FileIOType::kUmmapIO));

        /*
            Unlike StringMap, this can take in any input.
            It has the default value of empty.
            This is a required argument and thus does not have a default value.
        */

        AddOpt("-fn", common::args::ArgType::kString);

        /*
            This is a size input.
            Sizes have the format: [integer][bBkKmMgG]
            The following are examples of sizes:

            4, 4b, and 4B: 4 bytes
            4k and 4K: 4 kilobytes
            4m and 4M: 4 megabytes
            4g and 4G: 4 gigabytes

            The default value of this is 4*KB.
        */

        AddOpt("-bs", common::args::ArgType::kSize, 4 * KB);

        AddOpt("-wcnt", common::args::ArgType::kInt, 0);

        AddOpt("-rcnt", common::args::ArgType::kInt, 0);

        AddOpt("-stat", common::args::ArgType::kInt,0);

        AddOpt("-h", common::args::ArgType::kNone);

        /*Parse and verify the arguments*/
        ArgIter(argc, argv);
        VerifyArgs();
    }
};
void file_workload_generator(std::shared_ptr<bench::FileIO> &io,std::string fn , size_t bs,  uint32_t wcnt, uint32_t rcnt, uint32_t stat, uint32_t pos )
{
    //Posix Operations
    if (pos < 3 ) {
        io->Write(fn , bs, wcnt, rcnt);
        io->Read(fn , bs, wcnt, rcnt);
        io->Stat(fn,stat);
        
        io->AsyncWrite(fn , bs, wcnt, rcnt);
        io->AsyncRead(fn , bs, wcnt, rcnt);
        io->Stat(fn,stat);
    }  else if (pos > 2) { //Map operations
        io->Write(fn , bs, wcnt, rcnt);
        io->Read(fn , bs, wcnt, rcnt);
        io->Stat(fn,stat);
    }
    
}

int main(int argc, char **argv)
{
    //Get parameters
    ArgsParser args(argc, argv);
    int pos = args.GetIntOpt("-client");
    bench::FileIOType client = static_cast<bench::FileIOType>(args.GetIntOpt("-client"));
    std::string fn = args.GetStringOpt("-fn");
    size_t bs = args.GetSizeOpt("-bs");
    int wcnt = args.GetIntOpt("-wcnt");
    int rcnt = args.GetIntOpt("-rcnt");
    int stat = args.GetIntOpt("-stat");

    //Get I/O Client
    std::shared_ptr<bench::FileIO> io  = bench::FileIOFactory::Get(client);
    //Run the workload generator
    file_workload_generator(io,fn,bs,wcnt,rcnt,stat,pos);
    return 0;
}
