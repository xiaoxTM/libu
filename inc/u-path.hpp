#ifndef __U_PATH_HPP__
#define __U_PATH_HPP__

 #include "u-version.hpp"

/***
  u-path-system.hpp path processing
  Copyright (C) 2013  Renweu Gao

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/


#include "u-base.hpp"
#include "u-thread.hpp"
#include "u-string.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <climits>
#include <vector>
#include <list>
#include <typeinfo>

#include <mutex>
#include <thread>
#include <condition_variable>

#ifdef __GNUC__

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdlib>

#else
// for Microsoft C/C++ compiler

#endif

namespace u {

    static std::mutex _mutex;
    static std::condition_variable _cv;

    class path {
    private:
        char * _suffix;
        char * _prefix;
        char * _file;

    public:

        path(const std::string & _path) {
            assert(!_path.empty());
            size_t ps_pos = std::string::npos;
            size_t fs_pos = std::string::npos;
            ps_pos = _path.rfind(SYSTEM_PATH_SEPARATOR);
            fs_pos = _path.rfind(".");
            _file = NULL;
            _prefix = NULL;
            _suffix = NULL;

            if (fs_pos != std::string::npos && ps_pos != std::string::npos) {
                if (fs_pos < ps_pos + 1)
                    fs_pos = std::string::npos;
            }

            if (fs_pos != std::string::npos) {
                _suffix = u::string::dup(
                        _path.substr(fs_pos + 1, (_path.size() - fs_pos)));
            } else {
                fs_pos = _path.size();
            }

            if (ps_pos != std::string::npos) {
                if (ps_pos != 0)
                    _prefix = u::string::dup(_path.substr(0, ps_pos));
                ++ps_pos;
            } else {
                ps_pos = 0;
            }

            _file = u::string::dup(_path.substr(ps_pos, fs_pos - ps_pos));
        }

        ~path() {
            release();
        }

        /*get the private member, part should be {"prefix","file","suffix"}*/
        const char *get(const std::string &part) {
            if (part == "suffix")
                return _suffix;
            else if (part == "prefix")
                return _prefix;
            else if (part == "file")
                return _file;
            else
                return NULL;
        }

        /*get multi-parts of path, flag can be {u::P, u::F, u::S} and the combinations*/
        char *get(unsigned char flag) {
            std::string ret;
            char *ret_value = NULL;
            if ((flag & u::P) == u::P) {
                ret.append(_prefix);
            }
            if ((flag & u::F) == u::F) {
                if (!ret.empty()) {
                    ret.append(SYSTEM_PATH_SEPARATOR);
                }
                ret.append(_file);
            }
            if ((flag & u::S) == u::S) {
                if (!ret.empty()) {
                    ret.append(".");
                }
                ret.append(_suffix);
            }
            if (!ret.empty())
                ret_value = u::string::dup(ret);
            return ret_value;
        }

        /*release memory, mainly the members*/
        void release() {
            u::string::free(&_prefix);
            u::string::free(&_file);
            u::string::free(&_suffix);
        }

        /*check whether @name exists, flag={u::F, u::D}*/
        static bool exists(const std::string &filename, unsigned char flag = u::F) {
            bool found = false;
#ifdef __GNUC__
            if (!filename.empty()) {
                int status;
                struct stat st_buf;
                status = stat(filename.c_str(), &st_buf);
                if (status == 0) {
                    if ((flag == u::F && S_ISREG(st_buf.st_mode)) || (flag == u::D && S_ISDIR(st_buf.st_mode)))
                        found = true;
                }
            }
#else

#endif
            return (found);
        }

        /*check whether @name ends with @suffix*/
        static bool end_with(const std::string &filename, const std::string &suffix, bool ignore = false) {
            bool ret = false;
            long sub_len = static_cast<long> (filename.size()) - static_cast<long> (suffix.size());
            if (sub_len >= 0) {
                std::string sub = filename.substr(static_cast<size_t> (sub_len), suffix.size());
                std::string _suffix(suffix);
                if (ignore) {
                    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    std::transform(_suffix.begin(), _suffix.end(), _suffix.begin(), ::tolower);
                }
                ret = sub == _suffix;
            }
            return ret;
        }

        /*check whether @name begins with @prefix*/
        static bool begin_with(const std::string &filename, const std::string &prefix, bool ignore = false) {
            bool ret = false;
            long len = static_cast<long> (filename.size()) - static_cast<long> (prefix.size());
            if (len >= 0) {
                std::string sub = filename.substr(0, prefix.size());
                std::string _prefix(prefix);
                if (ignore) {
                    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    std::transform(_prefix.begin(), _prefix.end(), _prefix.begin(), ::tolower);
                }
                ret = sub == _prefix;
            }
            return ret;
        }

        /*combine @first and the latters to path, MUST be ended with NULL*/
        static std::string join(const std::vector<std::string> &others) {
            std::string ret;
            for (std::string value : others) {
                if (value.size() == 1 && value == SYSTEM_PATH_SEPARATOR)
                    continue;
                if (begin_with(value, SYSTEM_PATH_SEPARATOR)) {
                    if (end_with(ret, SYSTEM_PATH_SEPARATOR)) {
                        ret.append(value.substr(1));
                    } else {
                        ret.append(value);
                    }
                } else {
                    if (end_with(ret, SYSTEM_PATH_SEPARATOR)) {
                        ret.append(value);
                    } else {
                        ret.append(SYSTEM_PATH_SEPARATOR);
                        ret.append(value);
                    }
                }
            }
            return ret;
        }

        /*split @name to pieces with max numbers @num if @num not equal to -1 by @spliter*/
        static std::vector<char *> split(const std::string &name, const std::string &spliter = SYSTEM_PATH_SEPARATOR, int num = -1) {
            return u::string::split(name, spliter, num);
        }

        /*append @value to @part={prefix, file, suffix} in @name*/
        static char * append(const std::string &name, const std::string &value, const std::string &part) {
            assert(!name.empty() && !value.empty() && !part.empty());
            std::string ret(name);

            if (part == "suffix") {
                ret.append(value);
            } else if (part == "prefix") {
                size_t slash_pos = ret.rfind(SYSTEM_PATH_SEPARATOR);
                ret.insert(slash_pos + 1, value);
            } else if (part == "file") {
                size_t dot_pos = name.rfind(".");
                if (dot_pos != std::string::npos) {
                    ret.insert(dot_pos, value);
                }
            }
            return u::string::dup(ret);
        }

        /*replace @part={'prefix','file','suffix'} of @name with @value*/
        static char * replace(const std::string &name, const std::string &value, const std::string &part) {
            assert(!name.empty());
            size_t ps_pos = std::string::npos;
            size_t fs_pos = std::string::npos;
            ps_pos = name.rfind(SYSTEM_PATH_SEPARATOR);
            fs_pos = name.rfind(".");
            char *ret = NULL;

            if (fs_pos != std::string::npos && ps_pos != std::string::npos) {
                if (fs_pos < ps_pos + 1) {
                    fs_pos = std::string::npos;
                }
            }

            if (fs_pos == std::string::npos) {
                fs_pos = name.size();
            }

            if (ps_pos == std::string::npos) {
                ps_pos = 0;
            }

            if (part == "prefix") {
                ret = u::string::dup(u::path::join({value, name.substr(ps_pos + 1)}));
            } else if (part == "file") {
                std::string _path = u::path::join({name.substr(0, ps_pos + 1), value});
                ret = u::string::dup(_path + name.substr(fs_pos));
            } else if (part == "suffix") {
                ret = u::string::dup(name.substr(0, fs_pos + 1) + value);
            }
            return ret;
        }

        /*count how many ['/', '\'] in @name*/
        static size_t depth(const std::string &name) {
            size_t depth = 1;
            size_t pos = 0;
            size_t pre = 0;

            while ((pos = name.find(SYSTEM_PATH_SEPARATOR, pre)) != std::string::npos) {
                if (strcmp(name.substr(pre, (pos - pre)).c_str(), ".") != 0) {
                    ++depth;
                    if (strcmp(name.substr(pre, (pos - pre)).c_str(), "..") == 0)
                        depth -= 2;
                }
                pre = pos + 1;
            }
            return depth;
        }

        /*make @name, if @create_parent=true, non-exists parents will also be created*/
        static bool make_dir(const std::string &name, bool create_parent = false) {
            bool ret = true;
            if (!create_parent) {
                if (!u::path::exists(name, u::D)) {
                    if (mkdir(name.c_str(), 0774) != 0) {
                        ret = false;
                    }
                }
            } else {
                size_t pos;
                size_t prev = 0;
                while (true) {
                    pos = name.find(SYSTEM_PATH_SEPARATOR, prev);
                    prev = pos + 1;
                    std::string sub = name.substr(0, pos);
                    if (!sub.empty() and !exists(sub, u::D)) {
                        if (mkdir(sub.c_str(), 0774) != 0) {
                            ret = false;
                            break;
                        }
                    }
                    if (pos == std::string::npos)
                        break;
                }
            }

            return ret;
        }

        /*check if @name exists, if not @name will be created, if @create_parent=true, non-exists parents will also be created*/
        static bool confirm_dir(const std::string &name, bool create_parent = false) {
            bool ret = true;
            if (!path::exists(name, u::D)) {
                ret = path::make_dir(name, create_parent);
            }
            return ret;
        }

        template <typename T, class file_op>
        static bool rsync_walk(const std::string &root, file_op op, T &user_data, const std::string &parent = std::string()) {
#ifdef __GNUC__
            struct stat stbuf;
            struct dirent *dp;
            DIR *dfd;
            bool ret = true;
            char *dir = NULL;
            if (parent.empty())
                dir = u::string::dup(root);
            else
                dir = u::string::dup(u::path::join({root, parent}));
            if ((dfd = opendir(dir)) == NULL) {
                std::cout << "cannot open directory: " << dir << std::endl;
                ret = false;
                closedir(dfd);
            } else if (chdir(dir) != 0) {
                std::cout << "cannot enter directory: " << dir << std::endl;
                ret = false;
                size_t depth = path::depth(dir);
                std::string parent;
                while (depth != 0) {
                    parent.append("../");
                    --depth;
                }
                closedir(dfd);
            } else {
                while ((dp = readdir(dfd)) != NULL) {
                    if (stat(dp->d_name, &stbuf) == -1) {
                        std::cerr << "cannot access " << dp->d_name << "\n";
                        std::cerr << "warning: file " << dp->d_name << " will be escaped.\n";
                        continue;
                    }
                    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                        continue;
                    }
                    if ((stbuf.st_mode & S_IFMT) == S_IFREG) {
                        if (!op(root, parent, dp->d_name, u::F, user_data)) {
                            break;
                        }
                    } else if (S_ISDIR(stbuf.st_mode)) {
                        if (op(root, parent, dp->d_name, u::D, user_data)) {
                            char *name = NULL;
                            if (parent.empty())
                                name = u::string::dup(dp->d_name);
                            else
                                name = u::string::dup(u::path::join({parent, dp->d_name}));
                            bool go_on = rsync_walk(root, op, user_data, name);
                            delete [] name;
                            if (!go_on)
                                break;
                        } else {
                            break;
                        }
                    }
                }
                closedir(dfd);
                if (chdir("../") != 0) {
                    std::cout << "cannot change directory to parent of " << dir << std::endl;
                    ret = false;
                }
            }
            u::string::free(&dir);
#else
            // for Microsoft C/C++ compiler
#endif

            return ret;
        }

        /*in order to use this feature, -lpthread option needed*/

        template <typename T, class file_op>
        static void async_walk(u::ws::work_station &ws, const std::string &root, file_op op, T &user_data, const std::string &parent = std::string()) { //unknow bug here, 乱码输出
#ifdef __GNUC__
            struct stat stbuf;
            struct dirent *dp;
            DIR *dfd;
            char *dir = NULL;
            if (parent.empty())
                dir = u::string::dup(root);
            else
                dir = u::string::dup(u::path::join({root, parent}));
            if ((dfd = opendir(dir)) == NULL) {
                std::cout << "cannot open directory: " << dir << std::endl;
                closedir(dfd);
            } else if (chdir(dir) != 0) {
                std::cout << "cannot enter directory: " << dir << std::endl;
                size_t depth = path::depth(dir);
                std::string parent;
                while (depth != 0) {
                    parent.append("../");
                    --depth;
                }
                closedir(dfd);
            } else {
                std::mutex mutex;
                while ((dp = readdir(dfd)) != NULL) {
                    if (stat(dp->d_name, &stbuf) == -1) {
                        std::cerr << "cannot access " << dp->d_name << "\n";
                        std::cerr << "warning: file " << dp->d_name << " will be escaped.\n";
                        continue;
                    }
                    if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                        continue;
                    }
                    if ((stbuf.st_mode & S_IFMT) == S_IFREG) {
                        mutex.lock();
                        ws.run(op, std::string(root), std::string(parent), std::string(dp->d_name), u::F, user_data);
                        mutex.unlock();
                    } else if (S_ISDIR(stbuf.st_mode)) {
                        mutex.lock();
                        ws.run(op, std::string(root), std::string(parent), std::string(dp->d_name), u::D, user_data);
                        mutex.unlock();
                        char *name = NULL;
                        if (parent.empty())
                            name = u::string::dup(dp->d_name);
                        else
                            name = u::string::dup(u::path::join({parent, dp->d_name}));
                        async_walk(ws, root, op, user_data, name);
                        delete [] name;
                    }
                }
                closedir(dfd);
                if (chdir("../") != 0) {
                    std::cout << "cannot change directory to parent of " << dir << std::endl;
                }
            }
            u::string::free(&dir);
#else
            // for Microsoft C/C++ compiler
#endif

        }

        template <typename T, class file_op>
        static bool walk(size_t threads, const std::string &root, file_op op, T &user_data, const std::string &parent = std::string()) {
            bool ret = true;
            if (threads <= 1) {
                ret = rsync_walk(root, op, user_data, parent);
            } else {
                u::ws::work_station ws(threads);
                async_walk(ws, root, op, user_data, parent);
            }
            return ret;
        }

        /*count how many files in @fold*/

        static size_t size(const std::string &filename, unsigned char option = u::F) {
            auto count = [&option](const std::string &root, const std::string &parent, const std::string &name, unsigned char flag, size_t & _size)->bool {
                if ((flag & option) == flag)
                    _size += 1;
                return true;
            };
            size_t ret = 0;
            rsync_walk(filename, count, ret);
            return ret;
        }

    };

    template <typename T, class file_op>
    static bool walk(size_t threads, const std::string &root, file_op op, T &user_data, const std::string &parent = std::string()) {
        return path::walk<T, file_op>(threads, root, op, user_data, parent);
    }

    static char *path_cat(const std::string &root, const std::string &parent, const std::string &filename) {
        char *ret = NULL;
        if (!root.empty()) {
            ret = u::string::dup(root);
        }
        if (!parent.empty()) {
            char * tmp = ret;
            ret = u::string::dup(u::path::join({tmp, parent}));
            u::string::free(&tmp);
        }
        if (!filename.empty()) {
            char * tmp = ret;
            ret = u::string::dup(u::path::join({tmp, filename}));
            u::string::free(&tmp);
        }
        return ret;
    }
}

#endif
