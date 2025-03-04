#pragma once

#include <libruntime/Types.h>
#include <libruntime/ErrorOr.h>
#include <libruntime/RefCounted.h>

namespace libsystem
{

enum SeekOrigine
{
    BEGIN,
    CURRENT,
    END,
};

class Stream : public libruntime::RefCounted<Stream>
{
public:
    typedef int64_t Offset;

    Stream();

    ~Stream();

    virtual bool readable();

    virtual bool writable();

    virtual bool seekable();

    virtual libruntime::ErrorOr<size_t> read(void *buffer, size_t size);

    virtual libruntime::ErrorOr<size_t> write(const void *buffer, size_t size);

    virtual libruntime::ErrorOr<byte> read_byte();

    virtual libruntime::Error write_byte(byte byte);

    virtual libruntime::ErrorOr<size_t> seek(size_t offset, SeekOrigine origine);

    virtual libruntime::ErrorOr<size_t> tell();

    virtual void flush();
};

} // namespace libsystem
