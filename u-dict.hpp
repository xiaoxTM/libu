#ifndef __U_DICT_HPP__
#define __U_DICT_HPP__

/***
        u-dict.hpp dict datatype for libu
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

#include <map>
#include <vector>
#include <cassert>
#include <cstring>

namespace u {

    class dict {
    private:
        //struct key_compare {
        //    bool operator()(const std::string &lhs, const std::string &rhs) const {return lhs < rhs;}
        //};
        typedef std::map<std::string, std::pair<size_t, char *> >::iterator iterator;
        typedef std::map<std::string, std::pair<size_t, char *> >::const_iterator const_iterator;

        std::map<std::string, std::pair<size_t, char *>/*, key_compare*/> _map;

        std::vector<bool> set(bool replace, const std::vector<std::string> &keys) {
            return std::vector<bool>();
        }

        std::vector<bool> get(const std::vector<std::string> &keys) const {
            return std::vector<bool>();
        }

    public:

        dict() {
        }

        dict(const dict &d) : _map(d.map()) {
            for (std::map<std::string, std::pair<size_t, char *> >::iterator it = _map.begin(); it != _map.end(); ++it) {
                char *before = it->second.second;
                it->second.second = new char[it->second.first];
                memcpy(it->second.second, before, it->second.first);
            }
        }

        const std::map<std::string, std::pair<size_t, char *> > map() const {
            return _map;
        }

        template <typename T>
        bool get(const std::string &key, T &value) const {
            bool changed = false;
            const_iterator it = _map.find(key);
            if (it != _map.end() && it->second.second != NULL) {
                assert(sizeof (T) == it->second.first);
                value = *(reinterpret_cast<T*> (it->second.second));
                changed = true;
            }
            return changed;
        }

        template <typename T>
        T *get(const std::string &key) const {
            T *ret = NULL;
            const_iterator it = _map.find(key);
            if (it != _map.end()) {
                assert(sizeof (T) == it->second.first);
                ret = reinterpret_cast<T*> (it->second.second);
            }
            return ret;
        }

        template <class Arg, class... Args>
        std::vector<bool> get(const std::vector<std::string> &keys, Arg &arg, Args&... args) const {
            static const size_t len = sizeof...(args) + 1;
            assert(keys.size() == len);
            std::vector<bool> ret(1, false);
            ret[0] = get<Arg>(keys[0], arg);
            std::vector<std::string> sub(keys.begin() + 1, keys.end());
            std::vector<bool> _ret = get(sub, args...);
            ret.insert(ret.end(), _ret.begin(), _ret.end());
            return ret;
        }

        /**
         * BE CAREFUL when using this function with pointer datatype
         * OTHERWISE you will be suffering from memory leak. To avoid
         * this, use it the following way:
         *     double * pointer = new double[SIZE];
         *     dict.set(key, pointer, REPLACE);
         * After things done, DO do the following things to release memory
         * for each pointer type of data:
         *     double **p = dict.remove<double*>(key, false);
         *     if (p != NULL) {
         *         delete [] *p;
         *         delete [] p;
         *     }
         * ////////////////////////////////////////////
         * For the local variables, this function will make a copy of them,
         * so it is not necessary to worry about them (they will be release)
         * in the dis-construct function. NOTE for customlized type, the "="
         * operator must be overwritten. Or use treat it as pointer type
         * instead.
         */
        template <typename T>
        bool set(const std::string &key, T value, bool replace) {
            bool ret = false;
            size_t size = sizeof (T);
            char *value_ = new char[size];
            *(reinterpret_cast<T*> (value_)) = value;
            iterator it = _map.find(key);
            if (it != _map.end()) {
                assert(size == it->second.first);
                ret = true;
                if (replace) {
                    delete[] it->second.second;
                    it->second.second = value_;
                }
            } else {
                _map[key] = std::make_pair(size, value_);
            }
            return ret;
        }

        template <class Arg, class... Args>
        std::vector<bool> set(bool replace, const std::vector<std::string> &keys, Arg arg, Args... args) {
            std::vector<bool> ret(1, false);
            if (keys.size() != 0) {
                static const size_t len = sizeof...(Args) + 1;
                assert(keys.size() == len);
                size_t size = sizeof (Arg);
                char *value = new char[size];
                *(reinterpret_cast<Arg*> (value)) = arg;
                iterator it = _map.find(keys[0]);
                if (it != _map.end()) {
                    assert(size == it->second.first);
                    if (replace) {
                        delete [] it->second.second;
                        it->second.second = value;
                        ret[0] = true;
                    }
                } else {
                    _map[keys[0]] = std::make_pair(size, value);
                    ret[0] = true;
                }
                std::vector<std::string> sub(keys.begin() + 1, keys.end());
                if (keys.size() > 0) {
                    std::vector<bool> _ret = set(replace, sub, args...);
                    if (_ret.size() != 0) {
                        ret.insert(ret.end(), _ret.begin(), _ret.end());
                    }

                }
            }
            return ret;
        }

        template <typename T>
        T remove(const std::string &key, bool release) {
            T ret = NULL;
            iterator it = _map.find(key);
            if (it != _map.end()) {
                assert(sizeof (T) == it->second.first);
                if (release && it->second.second != NULL) {
                    delete [] it->second.second;
                } else {
                    ret = *reinterpret_cast<T*> (it->second.second);
                }
                _map.erase(it);
            }
            return ret;
        }

        /**
         * Note that this function will not work for pointer type variables
         * For avoiding memory leak, remove all the pointer type variables
         * before release and/or the dis-construct function
         */
        void release() {
            iterator it = _map.begin();
            while (it != _map.end()) {
                if (it->second.second != NULL) {
                    delete[] it->second.second;
                    it->second.second = NULL;
                    iterator next = it;
                    ++next;
                    _map.erase(it);
                    it = next;
                }
            }
        }

        ~dict() {
            release();
        }
    };
}

#endif
