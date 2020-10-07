#ifndef FILE_IO_FACTORY_CPP
#define FILE_IO_FACTORY_CPP

#include "posix_io.cpp"
#include "mmap_io.cpp"

#include <memory>
namespace bench {

enum class FileIOType {
	kNone, kPosixIO, kUmapIO, kmmapIO
};

class FileIOFactory {
	public:
	static std::shared_ptr<FileIO> Get(FileIOType type) {
		switch(type) {
			case FileIOType::kPosixIO: {
				return std::make_shared<PosixIO>();
			}
			case FileIOType::kmmapIO: {
				return std::make_shared<MmapIO>();
			}	

		}
	}
};

}

#endif