#include "pipe.h"

#include <util/stream/base.h>
#include <util/generic/yexception.h>

ssize_t TPipeHandle::Read(void* buffer, size_t byteCount) throw () {
#ifdef _win_
    return recv(Fd_, (char*) buffer, byteCount, 0);
#else
    return read(Fd_, buffer, byteCount);
#endif
}

ssize_t TPipeHandle::Write(const void* buffer, size_t byteCount) throw () {
#ifdef _win_
    return send(Fd_, (const char*) buffer, byteCount, 0);
#else
    return write(Fd_, buffer, byteCount);
#endif
}

bool TPipeHandle::Close() throw () {
    bool ok = true;
    if (Fd_ != INVALID_PIPEHANDLE) {
#ifdef _win_
        ok = closesocket(Fd_) == 0;
#else
        ok = close(Fd_) == 0;
#endif
    }
    Fd_ = INVALID_PIPEHANDLE;
    return ok;
}

void TPipeHandle::Pipe(TPipeHandle& reader, TPipeHandle& writer) {
    PIPEHANDLE fds[2];
#ifdef _win_
    int r = SocketPair(fds);
#else
    int r = pipe(fds);
#endif
    if (r < 0)
        ythrow TFileError() << "failed to create a pipe";

    TPipeHandle(fds[0]).Swap(reader);
    TPipeHandle(fds[1]).Swap(writer);
}

class TPipe::TImpl: public TRefCounted<TImpl, TAtomicCounter> {
public:
    TImpl()
        : Handle_(INVALID_PIPEHANDLE)
    {
    }

    TImpl(PIPEHANDLE fd)
        : Handle_(fd)
    {
    }

    inline ~TImpl() {
        Close();
    }

    bool IsOpen() {
        return Handle_.IsOpen();
    }

    inline void Close() {
        if (!Handle_.IsOpen())
            return;
        if (!Handle_.Close())
            ythrow TFileError() << "failed to close pipe";
    }

    TPipeHandle& GetHandle() throw () {
        return Handle_;
    }

    size_t Read(void* buffer, size_t count) {
        ssize_t r = Handle_.Read(buffer, count);
        if (r < 0)
            ythrow TFileError() << "failed to read from pipe";
        return r;
    }

    size_t Write(const void* buffer, size_t count) {
        ssize_t r = Handle_.Write(buffer, count);
        if (r < 0)
            ythrow TFileError() << "failed to write to pipe";
        return r;
    }
private:
    TPipeHandle Handle_;
};

TPipe::TPipe()
    : Impl_(new TImpl)
{
}

TPipe::TPipe(PIPEHANDLE fd)
    : Impl_(new TImpl(fd))
{
}

TPipe::~TPipe() throw () {
}

void TPipe::Close() {
    Impl_->Close();
}

PIPEHANDLE TPipe::GetHandle() const throw() {
    return Impl_->GetHandle();
}

bool TPipe::IsOpen() const throw () {
    return Impl_->IsOpen();
}

size_t TPipe::Read(void* buf, size_t len) {
    return Impl_->Read(buf, len);
}

size_t TPipe::Write(const void* buf, size_t len) {
    return Impl_->Write(buf, len);
}

void TPipe::Pipe(TPipe& reader, TPipe& writer) {
    TImplRef r(new TImpl());
    TImplRef w(new TImpl());

    TPipeHandle::Pipe(r->GetHandle(), w->GetHandle());

    r.Swap(reader.Impl_);
    w.Swap(writer.Impl_);
}
