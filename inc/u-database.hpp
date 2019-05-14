#ifndef __U_DATABASE_HPP__
#define __U_DATABASE_HPP__

/***
    u-database-system.hpp database related function
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

 #include "u-version.hpp"

#include <map>
#include <list>
#include <fstream>
#include "u-string.hpp"

namespace u {

    namespace kv {

        static std::map<std::string, std::string> * const load(const std::string &filename, size_t buff_size = 0) {
            std::ifstream ifs(filename.c_str());
            assert(!ifs.fail());
            char *buff = NULL;
            if (buff_size == 0) {
                buff_size = 1024;
            }
            buff = new char[buff_size];
            std::map<std::string, std::string> *ret = new std::map<std::string, std::string>();
            while (!ifs.eof()) {
                ifs.getline(buff, buff_size);
                std::streamsize count = ifs.gcount();
                if (count > 0) {
                    u::string::trim(buff, ' ', u::S | u::P);
                    if (strlen(buff) > 0 && buff[0] != '#') {//non-comment
                        std::vector<char *> key_value = u::string::split(buff, "=", 2);
                        assert(key_value.size() == 2);
                        u::string::trim(key_value[0], ' ');
                        u::string::trim(key_value[1], ' ');
                        assert(ret->find(key_value[0]) == ret->end());
                        (*ret)[key_value[0]] = key_value[1];
                    }
                }
            }
            ifs.close();
            u::string::free(&buff);
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, char* &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                value = u::string::dup(it->second);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, char &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<char>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, int &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<int>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, bool &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = true;
            if (it != map.end()) {
                assert(u::string::validate_bool(it->second));
                value = false;
                if (it->second == "on" || it->second == "true" || it->second == "yes") {
                    value = true;
                }
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, long &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<long>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, float &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<float>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, double &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<double>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, short &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<short>(it->second, &value);
            }
            return ret;
        }

        static bool get(const std::map<std::string, std::string> &map, const char *key, long double &value) {
            std::map<std::string, std::string>::const_iterator it = map.find(key);
            bool ret = false;
            if (it != map.end()) {
                ret = u::string::validate_type<long double>(it->second, &value);
            }
            return ret;
        }

//        template <typename T>
//        static bool get(const std::map<std::string, std::string> &map, const char *key, T &value) {
//            return get(map, key, value);
//        }
        
        template <typename T>
        static T get(const std::map<std::string, std::string> &map, const char *key) {
            T value;
            assert(get(map, key, value));
            return value;
        }

        template <typename T>
        static void set(std::map<std::string, std::string> &map, const char *key, T value) {
            std::map<std::string, std::string>::iterator it = map.find(key);
            if (it != map.end()) {
                char *tmp = NULL;
                assert(u::string::to_string<T>(value, &tmp));
                it->second = tmp;
            }
        }

        static bool save(const std::string &filename, const std::map<std::string, std::string> &map, size_t buff_size = 0) {
            bool ret = true;
            std::ifstream ifs(filename.c_str());
            std::list<const char *> _buff;
            if (ifs.fail()) {
                std::map<std::string, std::string>::const_iterator it = map.begin();
                while (it != map.end()) {
                    _buff.push_back(u::format("%s=%s", it->first.c_str(), it->second.c_str()));
                    ++it;
                }
            } else {
                char *buff = NULL;
                if (buff_size == 0) {
                    buff_size = 1024;
                }
                buff = new char[buff_size];
                while (!ifs.eof()) {
                    ifs.getline(buff, buff_size);
                    if (ifs.gcount() == 0) {
                        _buff.push_back("NULL");
                    } else {
                        u::string::trim(buff, ' ', u::S | u::P);
                        if (strlen(buff) > 0 && buff[0] != '#') {//non-comment
                            std::vector<char *> key_value = u::string::split(buff, "=", 2);
                            assert(key_value.size() == 2);
                            u::string::trim(key_value[0], ' ');
                            u::string::trim(key_value[1], ' ');
                            std::map<std::string, std::string>::const_iterator it = map.find(key_value[0]);
                            assert(it != map.end());
                            _buff.push_back(u::format("%s=%s", it->first.c_str(), it->second.c_str()));
                        } else if (strlen(buff) > 0 && buff[0] == '#') {
                            _buff.push_back(u::string::dup(buff));
                        } else {
                            _buff.push_back("NULL");
                        }
                    }
                }
                ifs.close();
                u::string::free(&buff);
            }
            std::ofstream ofs(filename.c_str());
            if (ofs.fail()) {
                ret = false;
            } else {
                std::list<const char*>::const_iterator it = _buff.begin();
                std::list<const char*>::const_iterator end = _buff.end();
                while (it != end) {
                    if (strcmp(*it, "NULL") == 0) {
                        ++it;
                        if (it != end)
                            ofs << std::endl;
                    } else {
                        ofs << (*it);
                        ++it;
                        if (it != end)
                            ofs << std::endl;
                    }
                }
            }
            _buff.clear();
            ofs.close();
            return ret;
        }
    }

}

#endif
