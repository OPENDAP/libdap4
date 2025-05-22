
#include "MarshallerThread.h"
#include <cstring>
#include <sstream>
#include <unistd.h>

using namespace libdap;

MarshallerThread::~MarshallerThread() {
    std::unique_lock<std::mutex> lock(d_out_mutex);
    d_out_cond.wait(lock, [this] { return d_child_thread_count == 0; });
}

void MarshallerThread::start_thread(std::ostream &out, char *byte_buf, std::streamsize bytes) {
    {
        std::unique_lock<std::mutex> lock(d_out_mutex);
        while (d_child_thread_count != 0)
            d_out_cond.wait(lock);
        d_child_thread_count++;
    }

    char *buffer_copy = new char[bytes];
    std::memcpy(buffer_copy, byte_buf, bytes);

    d_thread_future = std::async(std::launch::async, [this, &out, buffer_copy, bytes] {
        write_to_stream(out, buffer_copy, bytes, d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error);
    });
}

void MarshallerThread::start_thread(int fd, char *byte_buf, std::streamsize bytes) {
    {
        std::unique_lock<std::mutex> lock(d_out_mutex);
        while (d_child_thread_count != 0)
            d_out_cond.wait(lock);
        d_child_thread_count++;
    }

    char *buffer_copy = new char[bytes];
    std::memcpy(buffer_copy, byte_buf, bytes);

    d_thread_future = std::async(std::launch::async, [this, fd, buffer_copy, bytes] {
        write_to_fd(fd, buffer_copy, bytes, d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error);
    });
}

void MarshallerThread::start_thread_part(std::ostream &out, char *byte_buf, std::streamsize bytes) {
    {
        std::unique_lock<std::mutex> lock(d_out_mutex);
        while (d_child_thread_count != 0)
            d_out_cond.wait(lock);
        d_child_thread_count++;
    }

    char *buffer_copy = new char[bytes];
    std::memcpy(buffer_copy, byte_buf, bytes);

    d_thread_future = std::async(std::launch::async, [this, &out, buffer_copy, bytes] {
        write_part_to_stream(out, buffer_copy, bytes, d_out_mutex, d_out_cond, d_child_thread_count, d_thread_error);
    });
}

void MarshallerThread::write_to_stream(std::ostream &out, char *data, std::streamsize size, std::mutex &m,
                                       std::condition_variable &cv, int &count, std::string &err) {
    {
        std::lock_guard<std::mutex> lock(m);
        try {
            out.write(data, size);
            if (out.fail()) {
                std::ostringstream oss;
                oss << "Could not write data.";
                err = oss.str();
            }
        } catch (...) {
            err = "Exception while writing to stream.";
        }
        delete[] data;
        count = 0;
    }
    cv.notify_one();
}

void MarshallerThread::write_to_fd(int fd, char *data, std::streamsize size, std::mutex &m, std::condition_variable &cv,
                                   int &count, std::string &err) {
    {
        std::lock_guard<std::mutex> lock(m);
        auto bytes_written = write(fd, data, size);
        if (bytes_written != size) {
            std::ostringstream oss;
            oss << "Could not write all data to fd.";
            err = oss.str();
        }
        delete[] data;
        count = 0;
    }
    cv.notify_one();
}

void MarshallerThread::write_part_to_stream(std::ostream &out, char *data, std::streamsize size, std::mutex &m,
                                            std::condition_variable &cv, int &count, std::string &err) {
    {
        std::lock_guard<std::mutex> lock(m);
        try {
            if (size > 4) {
                out.write(data + 4, size - 4);
            } else {
                err = "Data too short to skip prefix.";
            }

            if (out.fail()) {
                std::ostringstream oss;
                oss << "Could not write partial data.";
                err = oss.str();
            }
        } catch (...) {
            err = "Exception while writing partial stream.";
        }
        delete[] data;
        count = 0;
    }
    cv.notify_one();
}
