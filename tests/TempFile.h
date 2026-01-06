//
// Created by James Gallagher on 1/4/26.
//

#ifndef LIBDAP_TEMPFILE_H
#define LIBDAP_TEMPFILE_H

#include <unistd.h>     // mkstemp, close, unlink
#include <stdexcept>
#include <string>
#include <utility>      // std::exchange
#include <vector>
#include <fstream>

namespace libdap {

class TempFile {
public:
    // pattern must contain at least 6 trailing 'X' characters, e.g. "/tmp/myapp-XXXXXX"
    explicit TempFile(std::string pattern,
                      std::ios::openmode mode = std::ios::out | std::ios::binary | std::ios::trunc) {
        create_and_open(std::move(pattern), mode);
    }

    ~TempFile() noexcept {
        cleanup();
    }

    TempFile(const TempFile &) = delete;
    TempFile &operator=(const TempFile &) = delete;

    TempFile(TempFile &&other) noexcept
        : path_(std::move(other.path_))
          , stream_(std::move(other.stream_))
          , unlink_on_destroy_(std::exchange(other.unlink_on_destroy_, false)) {
        other.path_.clear();
    }

    TempFile &operator=(TempFile &&other) noexcept {
        if (this != &other) {
            cleanup();
            path_ = std::move(other.path_);
            stream_ = std::move(other.stream_);
            unlink_on_destroy_ = std::exchange(other.unlink_on_destroy_, false);
            other.path_.clear();
        }
        return *this;
    }

    const std::string &path() const noexcept { return path_; }

    std::ofstream &stream() noexcept { return stream_; }
    const std::ofstream &stream() const noexcept { return stream_; }

    bool is_open() const noexcept { return stream_.is_open(); }
    void flush() noexcept { stream_.flush(); }

    // Optional: close early (file will still be unlinked in destructor unless you call release()).
    void close_stream() {
        if (stream_.is_open())
            stream_.close();
    }

    // Optional: keep the file on disk after this object dies.
    // Closes the stream and disables unlink-on-destroy, returning the path.
    std::string release() noexcept {
        close_stream();
        unlink_on_destroy_ = false;
        return path_;
    }

private:
    void create_and_open(std::string pattern, std::ios::openmode mode) {
        // mkstemp mutates its template buffer.
        std::vector<char> buf(pattern.begin(), pattern.end());
        buf.push_back('\0');

        int fd = ::mkstemp(buf.data());
        if (fd == -1) {
            throw std::runtime_error("mkstemp failed");
        }

        std::string created_path = buf.data();

        // We can't portably attach std::ofstream to an existing fd, so close it
        // and reopen by path using iostreams.
        if (::close(fd) != 0) {
            ::unlink(created_path.c_str());
            throw std::runtime_error("close(mkstemp fd) failed");
        }

        std::ofstream ofs;
        ofs.open(created_path, mode);
        if (!ofs.is_open() || !ofs) {
            ::unlink(created_path.c_str());
            throw std::runtime_error("ofstream open failed");
        }

        // Commit only after everything succeeded (strong exception safety).
        path_ = std::move(created_path);
        stream_ = std::move(ofs);
        unlink_on_destroy_ = true;
    }

    void cleanup() noexcept {
        // Never throw from cleanup/destructor.
        try {
            if (stream_.is_open()) {
                stream_.close();
            }
        } catch (...) {
            // swallow
        }

        if (unlink_on_destroy_ && !path_.empty()) {
            ::unlink(path_.c_str()); // ignore errors
        }

        // Reset to a benign state.
        unlink_on_destroy_ = false;
        path_.clear();
        // stream_ is already closed (or never opened); leaving it is fine.
    }

    std::string path_;
    std::ofstream stream_;
    bool unlink_on_destroy_ = false;
};

} // libdap

#endif //LIBDAP_TEMPFILE_H