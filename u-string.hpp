#ifndef __U_STRING_HPP__
#define	__U_STRING_HPP__

/***
  u-string.hpp base functions for libu
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

#include <cstring>
#include <cassert>
#include <climits>
#include <algorithm>
#include <sstream>
#include <vector>
#include "u-base.hpp"

namespace u {

    namespace string {

        /**
         ** Function  -- clone a string from a given string
         ** Parameters:
         **           -- value: a string to be cloned
         ** Return    -- a copy of @value
         ***/
        static char * dup(const std::string &value) {
            char *ret = NULL;
            if (!value.empty()) {
                ret = new char[value.size() + 1];
                strcpy(ret, value.c_str());
            }
            return ret;
        }

        /**
         ** Function  -- free a string, if string is null, nothing will done
         ** Parameters:
         **           -- strings: string to be release
         ***/
        static void free(char ** strings) {
            if (strings != NULL) {
                if (*strings != NULL) {
                    delete [] (*strings);
                    (*strings) = NULL;
                }
            }
        }

        static void free(std::vector<char *> &strings) {
            for (size_t i=0; i<strings.size(); ++i) {
                u::string::free(&(strings[i]));
            }
        }

        static void free(const std::vector<char **> &strings) {
            for (char ** string : strings) {
                u::string::free(string);
            }
        }

        /**
         ** Function  -- free a string array, any string will not be deal with
         ** Parameters:
         **           -- strings: string array to be released
         ***/
        static void vfree(char***strings) {
            if (strings != NULL && (*strings) != NULL) {
                int size = sizeof (strings) / sizeof (strings[0]);
                for (int i = 0; i < size; ++i) {
                    free(strings[i]);
                }
                delete [] (*strings);
                (*strings) = NULL;
            }
        }

        static void vfree(std::vector<char **> &strings) {
            for (char **&string : strings)
                vfree(&string);
        }

        static void vfree(const std::vector<char***> &strings) {
            for(char ***string :  strings) {
                vfree(string);
            }
        }

        /**
         ** Function  -- remove a given spliter in a string for the given piosition
         ** Parameters:
         **           -- string: string in which the @spliter will be removed
         **           -- spliter: the character will be removed in @string
         **           -- flag: position in which the @spliter will be removed
         **                    u::P for the prefix of @string
         **                    u::S for the suffix of @string
         ** Return    -- the number of @spliter have be removed
         ***/
        static size_t trim(char *string, unsigned char spliter, unsigned char flag = u::P | u::S) {
            assert(string != NULL);
            int total = 0;
            if ((flag & u::S) == u::S) {
                size_t size = strlen(string);
                while (size > 0 && string[size - 1] == spliter) {
                    ++total;
                    string[size - 1] = '\0';
                    --size;
                }
            }
            if ((flag & u::P) == u::P) {
                size_t size = strlen(string);
                size_t i = 0;
                size_t prefix = 0;
                while (i < size && string[i] == spliter) {
                    ++prefix;
                    ++i;
                }
		        if (prefix != 0) {
                    i = 0;
                    total += prefix;
                    size_t valiable = size - prefix;

                    while (valiable > 0) {
		        string[i] = string[i + prefix];
                        ++i;
                        --valiable;
                    }
                    string[size - prefix] = '\0';
		}
            }
            return total;
        }

        /**
         ** Function  -- remove a given spliter in a const string for the given piosition
         ** Parameters:
         **           -- string: string in which the @spliter will be removed
         **           -- spliter: the character will be removed in @string
         **           -- flag: position in which the @spliter will be removed
         **                    u::P for the prefix of @string
         **                    u::S for the suffix of @string
         ** Return    -- a new string that @spliter has been removed, if no @spliter removed
         **                    new @string will be returned
         ***/
        static char *trim(const std::string &string, unsigned char spliter, unsigned char flag = u::P | u::S) {
            char *dup = u::string::dup(string);
            if (dup != NULL) {
                size_t total = u::string::trim(dup, spliter, flag);
                if (total == string.size()) {// if all characters in string are @spliter
                    u::string::free(&dup);
                }
            }
            return dup;
        }

        static char *lower(const std::string &string) {
            size_t len = string.size();
            std::string ret(string);
            for (size_t i = 0; i < len; ++i) {
                if (isalpha(string[i])) {
                    ret[i] = static_cast<char> (tolower(string[i]));
                }
            }
            return u::string::dup(ret);
        }

        static char *upper(const std::string &string) {
            size_t len = string.size();
            std::string ret(string);
            for (size_t i = 0; i < len; ++i) {
                if (isalpha(string[i])) {
                    ret[i] = static_cast<char> (toupper(string[i]));
                }
            }
            return u::string::dup(ret);
        }

        /**
         ** Function  -- convert string to a given data type
         ** Parameters:
         **           -- value: string data type to be converted
         **           -- ret: a storage to store new data type value of @value
         ** Return    -- true if no error ocurred, otherwise false returned
         ***/
        template<typename T>
        static bool from_string(const std::string &value, T &ret) {
            bool bad = true;
            if (!value.empty()) {
                std::istringstream iss(value);
                iss >> ret;
                bad = (iss.fail() || !iss.eof());
            }
            return !bad;
        }

        template <typename T>
         static T from_string(const std::string &value) {
                T ret = std::numeric_limits<T>::max();
                T retrive;
                if (from_string<T>(value, retrive)) {
                    ret = retrive;
                }
                return ret;
            }

        /**
         ** Function  -- convert a given data type to string
         ** Parameters:
         **           -- value: data type to be converted
         **           -- ret: a storage to store coverted string if ret is not null
         ** Return    -- true if no error ocurred, otherwise false returned
         ***/
        template<typename T>
        static bool to_string(T value, char **ret) {
            std::ostringstream oss;
            oss << value;
            bool bad = oss.fail();
            if (ret != NULL) {
                u::string::free(ret);
                (*ret) = u::string::dup(oss.str());
            }
            return !bad;
        }

        template <typename T>
        static char *to_string(T value) {
            char *ret = NULL;
            if(!to_string<T>(value, &ret)) {
                u::string::free(&ret);
            }
            return ret;
        }

        static bool validate_printable(const std::string &value) {
            assert(value.size() == 1);
            return std::isprint(value[0]);
        }

        template <typename T>
        static bool validate_type(const std::string &value, T* ret = NULL) {
            T ivalue;
            bool result = from_string<T>(value, ivalue);
            if (ret != NULL)
                *ret = ivalue;
            return result;
        }

        static bool validate_bool(const std::string &value) {
            return (value == "true" || value == "false" || value == "on" || value == "off" || value == "yes" || value == "no");
        }

        static bool validate_int(const std::string &value) {
            return (!value.empty() && validate_type<int>(value));
        }

        static bool validate_uint(const std::string &value) {
            return (!value.empty() && value[0] != '-' && validate_type<unsigned int>(value));
        }

        static bool validate_short(const std::string &value) {
            return (!value.empty() && validate_type<short>(value));
        }

        static bool validate_ushort(const std::string &value) {
            return (!value.empty() && value[0] != '-' && validate_type<unsigned short>(value));
        }

        static bool validate_long(const std::string &value) {
            return (!value.empty() && validate_type<long>(value));
        }

        static bool validate_ulong(const std::string &value) {
            return (!value.empty() && value[0] != '-' && validate_type<unsigned long>(value));
        }

        static bool validate_float(const std::string &value) {
            return (!value.empty() && validate_type<float>(value));
        }

        static bool validate_double(const std::string &value) {
            return (!value.empty() && validate_type<double>(value));
        }

        static bool validate_long_double(const std::string &value) {
            return validate_type<long double>(value);
        }

        static bool validate_string(const std::string &value) {
            return true;
        }

        /*check whether @name ends with @suffix*/
       static bool end_with(const std::string &filename, const std::string &suffix, bool ignore) {
            bool ret = false;
            long sub_len = static_cast<long> (filename.size()) - static_cast<long> (suffix.size());
            if (sub_len >= 0) {
                std::string sub = filename.substr(static_cast<size_t> (sub_len), suffix.size());
                std::string _suffix(suffix);
                if (ignore) {
                    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    std::transform(_suffix.begin(), _suffix.end(), _suffix.begin(), ::tolower);
                }
                ret = (sub == _suffix);
            }
            return ret;
        }

        /*check whether @name begins with @prefix*/
        static bool begin_with(const std::string &filename, const std::string &prefix, bool ignore) {
            bool ret = false;
            long len = static_cast<long> (filename.size()) - static_cast<long> (prefix.size());
            if (len >= 0) {
                std::string sub = filename.substr(0, prefix.size());
                std::string _prefix(prefix);
                if (ignore) {
                    std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);
                    std::transform(_prefix.begin(), _prefix.end(), _prefix.begin(), ::tolower);
                }
                ret = (sub == _prefix);
            }
            return ret;
        }

        static bool begin_with(bool ignore, const std::string &name, const std::vector<std::string> &prefix) {
            bool ret = false;
            for (size_t i=0; i<prefix.size(); ++i) {
                if (! begin_with(name, prefix[i], ignore)) {
                    ret = true;
                    break;
                }
            }
            return ret;
        }

        static bool end_with(bool ignore, const std::string &name, const std::vector<std::string> &suffix) {
            bool ret = false;
            for (size_t i=0; i<suffix.size(); ++i) {
                if (end_with(name, suffix[i], ignore)) {
                    ret = true;
                    break;
                }
            }
            return ret;
        }

        /*split @name to pieces with max numbers @num if @num not equal to -1 by @spliter*/
        /*NOTE that T should either ``char *'' or ``std::string'' */
        template <typename T = char *>
        static std::vector<T> split(const std::string &name, const std::string &spliter = SYSTEM_PATH_SEPARATOR, int num = -1) {
            std::vector<T> ret;
            std::size_t pos = name.find(spliter);
            std::size_t prev = 0;
            if (num <= 0)
                num = INT_MAX;
            while (pos != std::string::npos && num > 0) {
                if (pos - prev != 0) {
                    ret.push_back(u::string::dup(name.substr(prev, pos - prev)));
                    --num;
                }
                prev = pos + spliter.size();
                pos = name.find(spliter, prev);
            }
            if (num > 0) {
                ret.push_back(u::string::dup(name.substr(prev)));
            }
            return ret;
        }
    }

}


#endif	/* U_STRING_SYSTEM_HPP */
