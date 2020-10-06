namespace bench
{

class FileIO
{
public:
    virtual void Read() = 0;
    virtual void Write() = 0;
    virtual void AsyncRead() = 0;
    virtual void AsyncWrite() = 0;
    virtual void Poll() = 0;
    virtual void Stat() = 0;
};

} // namespace bench
