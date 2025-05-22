
#ifndef MARSHALLERTHREAD_H_
#define MARSHALLERTHREAD_H_

#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <string>

namespace libdap {

class MarshallerThread {
private:
    std::mutex d_out_mutex;
    std::condition_variable d_out_cond;

    int d_child_thread_count = 0;
    std::string d_thread_error;

    std::future<void> d_thread_future;

public:
    MarshallerThread() = default;
    ~MarshallerThread();

    void start_thread(std::ostream &out, char *byte_buf, std::streamsize bytes_written);
    void start_thread(int fd, char *byte_buf, std::streamsize bytes_written);
    void start_thread_part(std::ostream &out, char *byte_buf, std::streamsize bytes_written);

    static void write_to_stream(std::ostream &out, char *data, std::streamsize size, std::mutex &m,
                                std::condition_variable &cv, int &count, std::string &err);
    static void write_to_fd(int fd, char *data, std::streamsize size, std::mutex &m, std::condition_variable &cv,
                            int &count, std::string &err);
    static void write_part_to_stream(std::ostream &out, char *data, std::streamsize size, std::mutex &m,
                                     std::condition_variable &cv, int &count, std::string &err);
};

} // namespace libdap

#endif
