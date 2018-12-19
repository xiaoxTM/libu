/*
* File:   u-string-system.hpp
* Author: xiaox
*
* Created on 2014/06/22, 17:30
*/

#ifndef __U_STRING_HPP__
#define	__U_STRING_HPP__

#include "u-version.hpp"

#include <cstring>
#include <cassert>
#include <climits>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "u-base.hpp"
namespace u {

    namespace string {
        enum fore_color_type {fore_black, fore_red, fore_green, fore_yellow, fore_blue, fore_magenta, fore_cyan, fore_white, fore_none};
        enum back_color_type {back_black, back_red, back_green, back_yellow, back_blue, back_magenta, back_cyan, back_white, back_none};
        enum font_style_type {font_reset, font_bold, font_underline, font_inverse, font_bold_off, font_underline_off, font_inverse_off, font_none};

        static std::map<fore_color_type, std::string> _fore_color_map;
        static std::map<back_color_type, std::string> _back_color_map;
        static std::map<font_style_type, std::string> _font_style_map;

        std::map<fore_color_type, std::string> _make_fore_color_map() {
            std::map<fore_color_type, std::string> map;
            map[fore_black] = std::string("30");
            map[fore_red] = std::string("31");
            map[fore_green] = std::string("32");
            map[fore_yellow] = std::string("33");
            map[fore_blue] = std::string("34");
            map[fore_magenta] = std::string("35");
            map[fore_cyan] = std::string("36");
            map[fore_white] = std::string("37");
            map[fore_none] = std::string();
            return map;
        }

        std::map<back_color_type, std::string> _make_back_color_map() {
            std::map<back_color_type, std::string> map;
            map[back_black] = std::string("40");
            map[back_red] = std::string("41");
            map[back_green] = std::string("42");
            map[back_yellow] = std::string("43");
            map[back_blue] = std::string("44");
            map[back_magenta] = std::string("45");
            map[back_cyan] = std::string("46");
            map[back_white] = std::string("47");
            map[back_none] = std::string();
            return map;
        }

        std::map<font_style_type, std::string> _make_font_style_map() {
            std::map<font_style_type, std::string> map;
            map[font_reset] = std::string("0");
            map[font_bold] = std::string("1");
            map[font_underline] = std::string("4");
            map[font_inverse] = std::string("7");
            map[font_bold_off] = std::string("21");
            map[font_underline_off] = std::string("24");
            map[font_inverse_off] = std::string("27");
            map[font_none] = std::string();
            return map;
        }

        const std::string fore_color(const fore_color_type index) {
            assert(index <= fore_none);
            if (_fore_color_map.size() == 0) {
                _fore_color_map = _make_fore_color_map();
            }
            return _fore_color_map[index];
        }

        const std::string back_color(const back_color_type index) {
            assert(index <= back_none);
            if (_back_color_map.size() == 0) {
                _back_color_map = _make_back_color_map();
            }
            return _back_color_map[index];
        }

        const std::string font_style(const font_style_type index) {
            assert(index <= font_none);
            if (_font_style_map.size() == 0) {
                _font_style_map = _make_font_style_map();
            }
            return _font_style_map[index];
        }
        
        // const static std::string fore_black = "30";
        // const static std::string fore_red = "31";
        // const static std::string fore_green = "32";
        // const static std::string fore_yellow = "33";
        // const static std::string fore_blue = "34";
        // const static std::string fore_magenta = "35";
        // const static std::string fore_cyan = "36";
        // const static std::string fore_white = "37";
        //
        // const static std::string back_black = "40";
        // const static std::string back_red = "41";
        // const static std::string back_green = "42";
        // const static std::string back_yellow = "43";
        // const static std::string back_blue = "44";
        // const static std::string back_magenta = "45";
        // const static std::string back_cyan = "46";
        // const static std::string back_white = "47";
        // const static std::string style_reset = "0";
        // const static std::string style_bold = "1"; // or style_bright
        // const static std::string style_underline = "4";
        // const static std::string style_inverse = "7";
        // const static std::string style_bold_off = "21";
        // const static std::string style_underline_off = "24";
        // const static std::string style_inverse_off = "27";

        static std::string styled_string(const std::string &text, const std::string &forecolor=std::string(), const std::string &backcolor=std::string(), const std::string &style=std::string(), bool reset=true) {
            assert(text.size() != 0);
            std::string ans("\033[");
            if (forecolor.size() > 0) {
                ans.append(forecolor);
            }
            if (backcolor.size() > 0) {
                if (ans[ans.size()-1] != '[') {
                    ans.append(";");
                }
                ans.append(backcolor);
            }
            if (style.size() > 0) {
                if (ans[ans.size()-1] != '[') {
                    ans.append(";");
                }
                ans.append(style);
            }
            ans.append("m" + text);
            if (reset) {
                ans.append("\033[0m");
            }

            return ans;
        }

        static std::string styled_string(const std::string &text, const fore_color_type &forecolor=fore_none, const back_color_type &backcolor=back_none, const font_style_type &style=font_none, bool reset=true) {
            assert(forecolor <= fore_none);
            assert(backcolor <= back_none);
            assert(style <= font_none);
            return styled_string(text, fore_color(forecolor), back_color(backcolor), font_style(style), reset);
        }

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
//
//        static void free(const std::vector<char **> &strings) {
//            for (char ** string : strings) {
//                u::string::free(string);
//            }
//        }
//
        /**
        ** Function  -- free a string array, any string will not be deal with
        ** Parameters:
        **           -- strings: string array to be released
        ***/
//        static void vfree(char***strings) {
//            if (strings != NULL && (*strings) != NULL) {
//                int size = sizeof (strings) / sizeof (strings[0]);
//                for (int i = 0; i < size; ++i) {
//                    free(strings[i]);
//                }
//                delete [] (*strings);
//                (*strings) = NULL;
//            }
//        }
//
//        static void vfree(std::vector<char **> &strings) {
//            for (char **&string : strings)
//            vfree(&string);
//        }
//
//        static void vfree(const std::vector<char***> &strings) {
//            for(char ***string :  strings) {
//                vfree(string);
//            }
//        }
//
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

        static void claim(int expression, const char *format, ...) {
            if (!expression) {
                char *msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::cout << msg << std::endl;
                u::string::free(&msg);
                assert(expression);
            }
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
