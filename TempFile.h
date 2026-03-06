//
// Created by James Gallagher on 1/4/26.
//

#ifndef LIBDAP_TEMPFILE_H
#define LIBDAP_TEMPFILE_H

#include <fstream>
#include <stdexcept>
#include <string>
#include <unistd.h> // mkstemp, close, unlink
#include <utility>  // std::exchange
#include <vector>

namespace libdap {

/** @brief RAII wrapper for a temporary file path and output stream.
 *
 * Creates a unique file, opens it as an `std::ofstream`, and unlinks it on
 * destruction unless ownership is released.
 */
class TempFile {
public:
    /** @brief Create and open a new temporary file.
     * @param pattern Template path ending with at least six `X` characters.
     * @param mode Open mode used for the output stream.
     */
    explicit TempFile(std::string pattern,
                      std::ios::openmode mode = std::ios::out | std::ios::binary | std::ios::trunc) {
        create_and_open(std::move(pattern), mode);
    }

    ~TempFile() noexcept { cleanup(); }

    TempFile(const TempFile &) = delete;
    TempFile &operator=(const TempFile &) = delete;

    /** @brief Move-construct from another TempFile.
     * @param other Source object whose file ownership is transferred.
     */
    TempFile(TempFile &&other) noexcept
        : path_(std::move(other.path_)), stream_(std::move(other.stream_)),
          unlink_on_destroy_(std::exchange(other.unlink_on_destroy_, false)) {
        other.path_.clear();
    }

    /** @brief Move-assign from another TempFile.
     * @param other Source object whose file ownership is transferred.
     * @return This instance.
     */
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

    /** @brief Get the path of the created temporary file.
     * @return Absolute or relative path used for the temporary file.
     */
    const std::string &path() const noexcept { return path_; }

    /** @brief Access the writable stream.
     * @return Writable `std::ofstream` bound to the temp file.
     */
    std::ofstream &stream() noexcept { return stream_; }

    /** @brief Access the writable stream as const.
     * @return Const `std::ofstream` bound to the temp file.
     */
    const std::ofstream &stream() const noexcept { return stream_; }

    /** @brief Check whether the stream is currently open.
     * @return True if the stream is open.
     */
    bool is_open() const noexcept { return stream_.is_open(); }

    /** @brief Flush buffered output to disk.
     */
    void flush() noexcept { stream_.flush(); }

    /** @brief Close the stream before destruction.
     *
     * The file is still unlinked by the destructor unless `release()` is called.
     */
    void close_stream() {
        if (stream_.is_open())
            stream_.close();
    }

    /** @brief Keep the temporary file on disk after object destruction.
     *
     * Closes the stream and disables automatic unlink behavior.
     * @return The path to the retained file.
     */
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

} // namespace libdap

#endif // LIBDAP_TEMPFILE_H
