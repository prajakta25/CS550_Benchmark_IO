#include "posix_io.cpp"
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
		}
	}
};

}
