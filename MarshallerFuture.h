
#ifndef MARSHALLERTHREAD_H_
#define MARSHALLERTHREAD_H_

#include <cstring>
#include <future>
#include <memory>
#include <stdexcept>
#include <unistd.h>

namespace libdap {

class MarshallerThread {
    std::future<std::streampos> d_ostream_future;
    std::future<ssize_t> d_fp_future;

public:
    MarshallerThread() = default;

    ~MarshallerThread() {
        if (d_ostream_future.valid()) {
            try {
                d_ostream_future.get();
            } catch (...) {
                // Suppress exceptions in destructor
            }
        }
        if (d_fp_future.valid()) {
            try {
                d_fp_future.get();
            } catch (...) {
                // Suppress exceptions in destructor
            }
        }
    }

    auto get_ostream_future() { return d_ostream_future.get(); }
    auto get_fp_future() { return d_fp_future.get(); }

    void start_thread(std::ostream &out, const char *byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), &out, num_bytes]() {
            out.write(buffer.get(), num_bytes);
            if (out.fail()) {
                throw std::runtime_error("Failed to write stream.");
            }
            return out.tellp();
        });
    }

    void start_thread(std::ostream &out, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(byte_buf), &out, num_bytes]() {
            out.write(buffer.get(), num_bytes);
            if (out.fail()) {
                throw std::runtime_error("Failed to write stream.");
            }
            return out.tellp();
        });
    }

    void start_thread_part(std::ostream &out, const char *byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), &out, num_bytes]() {
            if (num_bytes <= 4) {
                throw std::runtime_error("Data too short for partial write.");
            }
            out.write(buffer.get() + 4, num_bytes - 4);
            if (out.fail()) {
                throw std::runtime_error("Failed to write partial stream.");
            }
            return out.tellp();
        });
    }

    void start_thread_part(std::ostream &out, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_ostream_future.valid()) {
            d_ostream_future.get();
        }

        d_ostream_future = std::async(std::launch::async, [buffer = std::move(byte_buf), &out, num_bytes]() {
            if (num_bytes <= 4) {
                throw std::runtime_error("Data too short for partial write.");
            }
            out.write(buffer.get() + 4, num_bytes - 4);
            if (out.fail()) {
                throw std::runtime_error("Failed to write partial stream.");
            }
            return out.tellp();
        });
    }

    void start_thread(int fd, const char *byte_buf, std::streamsize num_bytes) {
        if (d_fp_future.valid()) {
            d_fp_future.get();
        }

        auto buffer_copy = std::make_unique<char[]>(num_bytes);
        std::memcpy(buffer_copy.get(), byte_buf, num_bytes);

        d_fp_future = std::async(std::launch::async, [buffer = std::move(buffer_copy), fd, num_bytes]() {
            auto written = write(fd, buffer.get(), num_bytes);
            if (written != num_bytes) {
                throw std::runtime_error("Failed to write all data to fd.");
            }
            return written;
        });
    }

    void start_thread(int fd, std::shared_ptr<const char> byte_buf, std::streamsize num_bytes) {
        if (d_fp_future.valid()) {
            d_fp_future.get();
        }

        d_fp_future = std::async(std::launch::async, [buffer = std::move(byte_buf), fd, num_bytes]() {
            auto written = write(fd, buffer.get(), num_bytes);
            if (written != num_bytes) {
                throw std::runtime_error("Failed to write all data to fd.");
            }
            return written;
        });
    }
};

} // namespace libdap

#endif
