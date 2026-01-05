//
// Created by James Gallagher on 1/4/26.
//

#ifndef LIBDAP_TEMPFILE_H
#define LIBDAP_TEMPFILE_H

#include <unistd.h>     // mkstemp, close, unlink
#include <fcntl.h>      // fcntl (optional)
#include <stdexcept>
#include <string>
#include <utility>      // std::exchange
#include <vector>

namespace libdap {

class TempFile {
public:
    // pattern must contain at least 6 trailing 'X' characters for mkstemp.
    // Example: "/tmp/myapp-XXXXXX"
    explicit TempFile(std::string pattern,
                      bool unlink_immediately = false)
        : fd_(-1) {
        // mkstemp modifies the buffer in-place, so we need a writable, NUL-terminated array.
        std::vector<char> buf(pattern.begin(), pattern.end());
        buf.push_back('\0');

        int fd = ::mkstemp(buf.data());
        if (fd == -1) {
            throw std::runtime_error("mkstemp failed");
        }

        fd_ = fd;
        path_ = buf.data();

        if (unlink_immediately) {
            // If unlink fails, still keep the file descriptor usable.
            // The file will remain on disk and path_ remains valid.
            ::unlink(path_.c_str());
        }
    }

    ~TempFile() {
        if (fd_ != -1) {
            ::close(fd_);
        }
        // Note: we do NOT unlink here by default, because the user may want the file to persist.
        // If you want auto-cleanup, pass unlink_immediately=true in the constructor.
    }

    TempFile(const TempFile &) = delete;
    TempFile &operator=(const TempFile &) = delete;

    TempFile(TempFile &&other) noexcept
        : fd_(std::exchange(other.fd_, -1)),
          path_(std::move(other.path_)) {
    }

    TempFile &operator=(TempFile &&other) noexcept {
        if (this != &other) {
            if (fd_ != -1)
                ::close(fd_);
            fd_ = std::exchange(other.fd_, -1);
            path_ = std::move(other.path_);
        }
        return *this;
    }

    int fd() const noexcept { return fd_; }
    const std::string &path() const noexcept { return path_; }

    // Close early if desired.
    void close() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
    }

private:
    int fd_;
    std::string path_;
};

} // libdap

#endif //LIBDAP_TEMPFILE_H