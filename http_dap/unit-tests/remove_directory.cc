//
// Created by James Gallagher on 2/22/23.
//

#include <cstring>
#include <dirent.h>
#include <string>

#include "Error.h"
#include "util.h"

using namespace std;

#define prolog string("HTTPCacheTest::").append(__func__).append("() - ")

namespace libdap {

/// There is an os-independent way of doing this in C++ 2017, but it is not portable in C++ 11.
/// @note This function is not thread safe.
/// jhrg 2/19/23
void remove_directory(const char *dir) {
    DIR *dp = opendir(dir);
    if (dp != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(dp))) {
            if (std::strcmp(entry->d_name, ".") != 0 && std::strcmp(entry->d_name, "..") != 0) {
                std::string path = std::string(dir) + "/" + entry->d_name;
                if (entry->d_type == DT_DIR) {
                    remove_directory(path.c_str());
                } else {
                    if (std::remove(path.c_str()) == -1)
                        throw Error(prolog + "Could not remove file " + path + " (" + std::strerror(errno) + ")",
                                    __FILE__, __LINE__);
                }
            }
        }
        closedir(dp);
    }
    if (std::remove(dir) == -1)
        throw Error(prolog + "Could not remove directory " + dir + " (" + std::strerror(errno) + ")", __FILE__,
                    __LINE__);
}

} // namespace libdap