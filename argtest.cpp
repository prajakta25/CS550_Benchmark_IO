//
// Created by lukemartinlogan on 8/30/20.
//

/*
 * String, StringMap, int, float, and size args
 * Option assertion
 * Default arguments
 * Invalid arguments
 * Empty param types
 * */

#include "arguments.h"
#define KB (1ul << 10)

enum class FileIOType
{
    kNone,
    kPosixIO,
    kAsyncPosixIO
};

class ExampleArgs : public common::args::ArgMap
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
        std::cout << "Usage: ./argtest -[param] value ...";
        std::cout << std::endl;

        std::cout << "-client [string]: Which I/O method to use. Posix or AsyncPosix." << std::endl;
        std::cout << "-fn [string]: The filename to perform I/O with" << std::endl;
        std::cout << "-bs [size]: The block size for I/O requests." << std::endl;
        std::cout << "-rcnt [int]: The number of read requests to generate" << std::endl;
        std::cout << "-wcnt [int]: The number of write requests to generate" << std::endl;
        std::cout << "-worthless [float]: A useless parameter to demonstrate how to input a float." << std::endl;
        std::cout << "-h: Print this help message" << std::endl;

        std::cout << std::endl;
        std::cout << "Format of size: [integer][bBkKmMgG]. Ex: 4k and 4K, 4g and 4G, etc." << std::endl;
    }

    /*
        This function actually parses the argument vector
    */

    ExampleArgs(int argc, char **argv)
    {
        /*
            A StringMap converts a string to an enumeration.
            In this case, the enumeration is of type FileIOType
            This is a required argument and thus does not have a default value.
        */
        AddOpt("-client", common::args::ArgType::kStringMap);
        AddStringMapVal("-client", "Posix", static_cast<int>(FileIOType::kPosixIO));
        AddStringMapVal("-client", "AsyncPosix", static_cast<int>(FileIOType::kAsyncPosixIO));

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

        /*
            These integer inputs with default value of 0.
            Technically, it can be positive or negative.

        */
        AddOpt("-wcnt", common::args::ArgType::kInt, 0);
        AddOpt("-rcnt", common::args::ArgType::kInt, 0);

        /*
            This is a float input.
            At the moment, These have no meaning; I just need to show you all the parser can do.
            Float may come up later
        */
        AddOpt("-worthless", common::args::ArgType::kFloat);

        /*
            This is a flag input. It is either set or it is not.
        */
        AddOpt("-h", common::args::ArgType::kNone);

        /*Parse and verify the arguments*/
        ArgIter(argc, argv);
        VerifyArgs();
    }
};

int main(int argc, char **argv)
{
    ExampleArgs args(argc, argv);
    FileIOType client = static_cast<FileIOType>(args.GetIntOpt("-client"));
    std::string fn = args.GetStringOpt("-fn");
    size_t bs = args.GetSizeOpt("-bs");
    int wcnt = args.GetIntOpt("-wcnt");
    int rcnt = args.GetIntOpt("-rcnt");
    float worthless = args.GetFloatOpt("-worthless");
}
