#include <iostream>

namespace bench
{

class FileIO
{
public:
    virtual void Read(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt) = 0;
    virtual void Write(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt) = 0;
    virtual void AsyncRead(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt) = 0;
    virtual void AsyncWrite(std::string fn , size_t bs, uint32_t wcnt, uint32_t rcnt) = 0;
    virtual void Poll() = 0;
    virtual void Stat(std::string fn, uint32_t st) = 0;
};

} // namespace bench
