#ifndef __U_OPTIONS_HPP__
#define __U_OPTIONS_HPP__

/***
    u-options.hpp options parse function
    Copyright (C) 2013  Renwu Gao

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

#include <functional>
#include <typeinfo>
#include <iomanip>

#include "u-log.hpp"

namespace u {
    namespace options {

        static std::map<const std::string, std::string> init_map() {
            std::map<const std::string, std::string> maps;
            maps[typeid(char).name()]   = std::string("char");
            maps[typeid(bool).name()]   = std::string("bool");
            maps[typeid(short).name()]  = std::string("short");
            maps[typeid(unsigned short).name()] = std::string("ushort");
            maps[typeid(int).name()] = std::string("int");
            maps[typeid(unsigned int).name()] = std::string("uint");
            maps[typeid(long).name()] = std::string("long");
            maps[typeid(unsigned long).name()] = std::string("ulong");
            maps[typeid(float).name()] = std::string("float");
            maps[typeid(double).name()] = std::string("double");
            maps[typeid(long double).name()] = std::string("long-double");
            maps[typeid(char*).name()] = std::string("string");
            maps[typeid(std::string).name()] = std::string("string");
            return maps;
        }

        static std::map<const std::string, std::string> typemap = init_map();

        static std::map<const std::string, std::function<bool(const std::string &)> > init() {
            std::map <const std::string, std::function<bool(const std::string &)> > trans;
            trans[typeid(char).name()] = u::string::validate_printable;
            trans[typeid(bool).name()] = u::string::validate_bool;
            trans[typeid(short).name()] = u::string::validate_short;
            trans[typeid(unsigned short).name()] = u::string::validate_ushort;
            trans[typeid(int).name()] = u::string::validate_int;
            trans[typeid(unsigned int).name()] = u::string::validate_uint;
            trans[typeid(long).name()] = u::string::validate_long;
            trans[typeid(unsigned long).name()] = u::string::validate_ulong;
            trans[typeid(float).name()] = u::string::validate_float;
            trans[typeid(double).name()] = u::string::validate_double;
            trans[typeid(long double).name()] = u::string::validate_long_double;
            trans[typeid(std::string).name()] = u::string::validate_string;
            trans[typeid(char *).name()] = u::string::validate_string;
            return trans;
        }

        static std::map<const std::string, std::function<bool(const std::string &)> > transfer = init();

        class entry {
        private:
            char* _vname;
            char* _type;
            char* _long;
            char* _short;
            char* _description;

            /*bool _option_validate(const char* type_, const char *default_) {
                assert(type_ != NULL);
                assert(default_ != NULL);

                std::map<const std::string, std::function<bool(const std::string &)> >::iterator iter = transfer.find(type_);
                return (iter == transfer.end() ? false : (iter->second(default_)));
            }*/

            entry(char* vname_, const char* type_,
                const char* long_, const char* short_, const char *default_, const char *description_) {
                assert(vname_ != NULL);
                assert(type_ != NULL && (long_ != NULL || short_ != NULL));
                //assert(_option_validate(type_, default_));
                _vname = vname_;
                if (strcmp(type_, typeid(char *).name()) == 0 || strcmp(type_, typeid(std::string).name()) == 0) {
                    char **vname = reinterpret_cast<char **> (_vname);
                    *vname = NULL;
                }
                _type = u::string::dup(type_);
                _long = NULL;
                if (long_ != NULL) {
                    _long = u::string::dup(long_);
                }
                _short = NULL;
                if (short_ != NULL) {
                    _short = u::string::dup(short_);
                }
                _description = NULL;
                if (description_ != NULL) {
                    _description = u::string::dup(description_);
                }

                value(default_);
            }

        public:
            void release() {
                u::string::free(&_type);
                u::string::free(&_long);
                u::string::free(&_short);
            }

            template <typename T>
            static entry * create(T* vname_, const char* long_, const char* short_, T default_, const char *description_ = NULL) {
                assert(vname_ != NULL && (short_ != NULL || long_ != NULL));
                char *default_ptr = u::string::to_string<T>(default_);
                if (default_ptr == NULL) {
                    default_ptr = u::string::dup("$NULL");
                }
                entry *ret = new entry(reinterpret_cast<char *> (vname_), typeid(T).name(), long_, short_, default_ptr, description_);
                return ret;
            }

            ~entry() {
                release();
            }

            const char * type() const {
                return _type;
            }

            const char * get(const std::string &flag) const {
                u_assert((flag == "long" || flag == "short" || flag == "description" || flag == "type"), u::format("FATAL: unknown flag.", flag.c_str()));
                const char *ret = NULL;
                if (flag == "long") {
                    ret = _long;
                } else if (flag == "short") {
                    ret = _short;
                } else if (flag == "description") {
                    ret = _description;
                } else {
                    ret = _type;
                }
                return ret;
            }

            char *value() {
                assert(_type != NULL);
                char *ret = NULL;
                if (strcmp(_type, typeid(char).name()) == 0) {
                    ret = new char(*_vname);
                } else if (strcmp(_type, typeid(bool).name()) == 0) {
                    bool* bool_ret = reinterpret_cast<bool*> (_vname);
                    if(*bool_ret)
                        ret = u::string::dup("true");
                    else
                        ret = u::string::dup("false");
                } else if (strcmp(_type, typeid(short).name()) == 0) {
                    short* short_ret = reinterpret_cast<short*> (_vname);
                    assert(u::string::to_string<short>(*short_ret, &ret));
                } else if (strcmp(_type, typeid(unsigned short).name()) == 0) {
                    unsigned short* ushort_ret = reinterpret_cast<unsigned short*> (_vname);
                    assert(u::string::to_string<unsigned short>(*ushort_ret, &ret));
                } else if (strcmp(_type, typeid(int).name()) == 0) {
                    int* int_ret = reinterpret_cast<int *> (_vname);
                    assert(u::string::to_string<int>(*int_ret, &ret));
                } else if (strcmp(_type, typeid(unsigned int).name()) == 0) {
                    unsigned int* uint_ret = reinterpret_cast<unsigned int*> (_vname);
                    assert(u::string::to_string<unsigned int>(*uint_ret, &ret));
                } else if (strcmp(_type, typeid(long).name()) == 0) {
                    long* long_ret = reinterpret_cast<long *> (_vname);
                    assert(u::string::to_string<long>(*long_ret, &ret));
                } else if (strcmp(_type, typeid(unsigned long).name()) == 0) {
                    unsigned long* ulong_ret = reinterpret_cast<unsigned long*> (_vname);
                    assert(u::string::to_string<unsigned long>(*ulong_ret, &ret));
                } else if (strcmp(_type, typeid(float).name()) == 0) {
                    float* float_ret = reinterpret_cast<float*> (_vname);
                    assert(u::string::to_string<float>(*float_ret, &ret));
                } else if (strcmp(_type, typeid(double).name()) == 0) {
                    double* double_ret = reinterpret_cast<double *> (_vname);
                    assert(u::string::to_string<double>(*double_ret, &ret));
                } else if (strcmp(_type, typeid(long double).name()) == 0) {
                    long double* long_double_ret = reinterpret_cast<long double*> (_vname);
                    assert(u::string::to_string<long double>(*long_double_ret, &ret));
                } else if (strcmp(_type, typeid(char *).name()) == 0 || strcmp(_type, typeid(std::string).name()) == 0) { // may be memory leak here. i.e. more than twice calling value
                    char **vname = reinterpret_cast<char **> (_vname);
                    if (*vname != NULL) ret = u::string::dup(*vname);
                } else {
                    const bool UNKNOWN_DATA_TYPE = false;
                    u_assert(UNKNOWN_DATA_TYPE, u::format("FATAL: unknown data type: %s.", _type));
                }
                return ret;
            }

            void value(const char* value_) {
                assert(value_ != NULL);

                if (strcmp(_type, typeid(char).name()) == 0) {
                    u_assert(u::string::validate_printable(value_), u::format("FATAL: non-printable character: %c", value_));
                    *_vname = *value_;
                } else if (strcmp(_type, typeid(bool).name()) == 0) {
                    u_assert(strcmp(value_, "true") == 0 || strcmp(value_, "off") ||
                            strcmp(value_, "false") == 0 || strcmp(value_, "on"), u::format("FATAL: unknown boolean type value: %s", value_));

                    bool* ret = reinterpret_cast<bool*> (_vname);
                    if (strcmp(value_, "true") == 0 || strcmp(value_, "on") == 0) {
                        *ret = true;
                    } else {
                        *ret = false;
                    }
                } else if (strcmp(_type, typeid(short).name()) == 0) {
                    short* ret = reinterpret_cast<short*> (_vname);
                    u_assert(u::string::validate_type<short>(value_, ret), u::format("FATAL: %s is not short data type", value_));
                } else if (strcmp(_type, typeid(unsigned short).name()) == 0) {
                    unsigned short* ret = reinterpret_cast<unsigned short*> (_vname);
                    u_assert(u::string::validate_type<unsigned short>(value_, ret), u::format("FATAL: %s is not unsigned short data type", value_));
                } else if (strcmp(_type, typeid(int).name()) == 0) {
                    int* ret = reinterpret_cast<int *> (_vname);
                    u_assert(u::string::validate_type<int>(value_, ret), u::format("FATAL: %s is not int data type", value_));
                } else if (strcmp(_type, typeid(unsigned int).name()) == 0) {
                    unsigned int* ret = reinterpret_cast<unsigned int*> (_vname);
                    u_assert(u::string::validate_type<unsigned int>(value_, ret), u::format("FATAL: %s is not unsigned int data type", value_));
                } else if (strcmp(_type, typeid(long).name()) == 0) {
                    long* ret = reinterpret_cast<long *> (_vname);
                    u_assert(u::string::validate_type<long>(value_, ret), u::format("FATAL: %s is not long data type", value_));
                } else if (strcmp(_type, typeid(unsigned long).name()) == 0) {
                    unsigned long* ret = reinterpret_cast<unsigned long*> (_vname);
                    u_assert(u::string::validate_type<unsigned long>(value_, ret), u::format("FATAL: %s is not unsigned long data type", value_));
                } else if (strcmp(_type, typeid(float).name()) == 0) {
                    float* ret = reinterpret_cast<float*> (_vname);
                    u_assert(u::string::validate_type<float>(value_, ret), u::format("FATAL: %s is not float data type", value_));
                } else if (strcmp(_type, typeid(double).name()) == 0) {
                    double* ret = reinterpret_cast<double *> (_vname);
                    u_assert(u::string::validate_type<double>(value_, ret), u::format("FATAL: %s is not double data type", value_));
                } else if (strcmp(_type, typeid(long double).name()) == 0) {
                    long double* ret = reinterpret_cast<long double*> (_vname);
                    u_assert(u::string::validate_type<long double>(value_, ret), u::format("FATAL: %s is not long double", value_));
                } else if (strcmp(_type, typeid(char *).name()) == 0 || strcmp(_type, typeid(std::string).name()) == 0) { // may be memory leak here. i.e. more than twice calling value
                    char **vname = reinterpret_cast<char **> (_vname);
                    u::string::free(vname);
                    if (strcmp(value_, "$NULL") != 0)
                        *vname = u::string::dup(value_);
                } else {
                    const bool UNKNOWN_TYPE = false;
                    u_assert(UNKNOWN_TYPE, u::format("FATAL: unknown data type: %s.", _type));
                }
            }
        };

        /*
         *@function find_options: find option and index (po pair) from @options that has the key @key
         *    NOTE: return vector of pair is because, in the case of short form, user can specify multiply
         *          options like:
         *                      -psdfj
         *          that in a complete form of -p -s -d -f -j
         */
        static std::vector<std::pair<std::string, int> > find_options(const std::vector<entry*> &option, const std::string &key) {
            assert(key.size() >= 2);
            size_t len = option.size();
            std::string flag;
            std::vector<std::pair<std::string, int> > pos; // = new std::vector<std::pair<std::string, int> >();
            if (key[1] == '-') {
                assert(key.size() >= 3);
                flag = "long";
                pos.push_back(std::pair<std::string, int>(key, -1));
            } else {
                for (size_t i = 1; i < key.size(); ++i) { // for options like -psdfj each of letter is a bool type switch
                    pos.push_back(std::pair<std::string, int>(std::string("-") + key[i], -1));
                }
                flag = "short";
            }
            for (size_t i = 0; i < pos.size(); ++i) {
                for (size_t k = 0; k < len; ++k) {
                    const char *value = option[k]->get(flag);
                    if (value != NULL && pos[i].first == value) {
                        pos[i].second = k;
                        break;
                    }
                }
            }
            return pos;
        }

        static void help(const char* program, const std::vector<entry*> &option) {
            if (program != NULL) {
                size_t len = option.size();
                const char *tmp = NULL;
                std::vector<std::vector<std::string> > information;
                std::vector<int> length(4, 0);
                {// head of help information
                    information.push_back(std::vector<std::string>());
                    information[0].push_back("#### LONG");
                    length[0] = static_cast<int>(information[0][0].size());
                    information[0].push_back("SHORT");
                    length[1] = static_cast<int>(information[0][1].size());
                    information[0].push_back("TYPE");
                    length[2] = static_cast<int>(information[0][2].size());
                    information[0].push_back("DEFAULT");
                    length[3] = static_cast<int>(information[0][3].size());
                    information[0].push_back("DESCRIPTION");
                }
                for (size_t i = 0; i < len; ++i) {
                    std::vector<std::string> info;
                    tmp = option[i]->get("long");
                    if (tmp != NULL)
                        info.push_back(std::string("++++ ") + tmp);
                    else
                        info.push_back("++++ <no-long>");
                    length[0] = std::max(length[0], static_cast<int>(info[0].size()));

                    tmp = option[i]->get("short");
                    if (tmp != NULL)
                        info.push_back(tmp);
                    else
                        info.push_back("<no-short>");
                    length[1] = std::max(length[1], static_cast<int>(info[1].size()));

                    info.push_back(std::string("<") + typemap[option[i]->get("type")] + std::string(">"));

                    length[2] = std::max(length[2], static_cast<int>(info[2].size()));

                    tmp = option[i]->value();
                    if (tmp != NULL)
                        info.push_back(tmp);
                    else
                        info.push_back("<NULL>");
                    length[3] = std::max(length[3], static_cast<int>(info[3].size()));

                    tmp = option[i]->get("description");
                    if (tmp != NULL)
                        info.push_back(tmp);
                    else
                        info.push_back("<no-description>");
                    information.push_back(info);
                }
                // print information of each column
                std::cout << std::left << std::setw(length[0]) << information[0][0] << "  ";
                std::cout << std::left << std::setw(length[1]) << information[0][1] << "  ";
                std::cout << std::right << std::setw(length[2]) << information[0][2] << "  ";
                std::cout << std::right << std::setw(length[3]) << information[0][3] << "  ";
                std::cout << information[0][4] << std::endl << std::endl;
                for (size_t i=1; i<information.size(); ++i) {
                    std::cout << std::left << std::setw(length[0]) << information[i][0] << ", ";
                    std::cout << std::left << std::setw(length[1]) << information[i][1] << ", ";
                    std::cout << std::right << std::setw(length[2]) << information[i][2] << ", ";
                    std::cout << std::right << std::setw(length[3]) << information[i][3] << ": ";
                    std::cout << information[i][4] << std::endl;
                }
            }
        }

        static bool check_helper(const entry* lhs, const entry* rhs) {
            assert(lhs != NULL && rhs != NULL);
            bool ret = true;
            const char *l_long = lhs->get("long");
            const char *l_short = lhs->get("short");
            const char *r_long = rhs->get("long");
            const char *r_short = rhs->get("short");
            if (l_long != NULL && r_long != NULL) {
                ret = (strcmp(l_long, r_long) != 0);
            }
            if (ret && l_short != NULL && r_short != NULL) {
                ret = (strcmp(l_short, r_short) != 0);
            }
            return ret;
        }

        static bool check(const std::vector<entry *> &options) {
            bool ret = true;
            size_t size = options.size();
            if (size > 0) {
                for (size_t i = 0; i < size - 1; ++i) {
                    assert(options[i] != NULL);
                    for (size_t j = i + 1; j < size; ++j) {
                        if (!check_helper(options[i], options[j])) {
                            ret = false;
                            break;
                        }
                    }
                    if (!ret) {
                        break;
                    }
                }
            }
            return ret;
        }

        static bool parse(int &argc, char **&argv, std::vector<entry*> &option) {
            assert(check(option));
            std::vector<char *> *marker = new std::vector<char *>();
            std::vector<std::pair<std::string, int> > map;
            marker->push_back(u::string::dup(argv[0]));
            for (int i = 1; i < argc; ++i) {
                if (argv[i][0] == '-') { // if is option, like long term: --preview or short term: -p
                    if (map.empty() || strcmp(option[map[map.size()-1].second]->type(), typeid(bool).name()) == 0) { // if one option done or, in case like: -psd -f <-- -j
                        map.clear(); // in case of  -psd -f <-- -j, clear previous option
                        std::vector<std::string> split = u::string::split<std::string>(argv[i], "=", 2);
                        if (split.size() == 2) { // if option has the form of --option=<value>
                            map = find_options(option, split[0]);
                            u_assert (map[0].second != -1, u::format("FATAL: unknown options %s in form of %s=%s.", split[0].c_str(), split[0].c_str(), split[1].c_str()));
                            option[map[0].second]->value(split[1].c_str());
                            map.clear();
                        } else {
                            map = find_options(option, argv[i]);
                            size_t len = map.size();
                            for (size_t n = 0; n < len; ++n) { // for each options in case of abbreiation form of several switch  options
                                if (map[n].second == -1) { // if not found, that is, it is not options actually but value of an option, like negative float number, e.g., -23.93
                                    marker->push_back(u::string::dup(map[n].first));
                                } else {
                                    if (strcmp(option[map[n].second]->type(), typeid(bool).name()) == 0)  option[map[n].second]->value("on");
                                }
                            }
                        }
                    } else { // negative number, e.g., -23.93
                        option[map[0].second]->value(argv[i]);
                        map.clear();
                    }
                } else { // value of options
                    if (!map.empty()) {
                        for (size_t j=0; j<map.size(); ++j) option[map[j].second]->value(argv[i]); // set all options to the same value, options should have the same data type
                        map.clear();
                    } else {
                        marker->push_back(u::string::dup(argv[i]));
                    }
                }
            }
            argc = marker->size();
            if (argc == 0) {
                argv = NULL;
            } else {
                argv = &(*marker)[0];
            }
            return marker->empty();
        }
    }
}

#endif
