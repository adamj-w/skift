#pragma once

#include <libsystem/Assert.h>

namespace libruntime
{

template <typename T>
class RefCounted
{
    // FIXME: make ref and deref atomics

private:
    int _refcount;
    bool _orphan;

public:
    RefCounted()
    {
        _refcount = 1;
        _orphan = false;
    }

    virtual ~RefCounted()
    {
        if (!_orphan)
        {
            assert(_refcount == 0);
        }
    }

    // Find a better name for this...
    void make_orphan()
    {
        _orphan = true;
    }

    void ref()
    {
        if (!_orphan)
        {
            assert(_refcount > 0);

            _refcount++;
        }
    }

    void deref()
    {
        if (!_orphan)
        {
            assert(_refcount > 0);

            _refcount--;

            if (_refcount == 0)
            {
                delete static_cast<T *>(this);
            }
        }
    }

    int refcount()
    {
        if (!_orphan)
        {
            return _refcount;
        }
        else
        {
            return 0;
        }
    }
};

} // namespace libruntime
