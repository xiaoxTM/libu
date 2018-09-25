#ifndef __U_OPTIONS_HPP__
#define __U_OPTIONS_HPP__

#include "u-version.hpp"

/***
    u-options-system.hpp options parse function
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


#include <vector>
#include <functional>

#include "u-string.hpp"

namespace u {
    namespace options {
        // For spec later than c++ 11

        static std::vector<std::pair<std::function<bool(const std::string &)>, std::string> > init() {
            std::vector < std::pair < std::function<bool(const std::string &)>, std::string> > trans;// = new std::vector < std::pair < std::function<bool(const std::string &)>, std::string> >();
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_printable, "char"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_bool, "bool"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_short, "short"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_ushort, "ushort"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_int, "int"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_uint, "uint"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_long, "long"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_ulong, "ulong"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_float, "float"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_double, "double"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_long_double, "long-double"));
            trans.push_back(std::pair < std::function<bool(const std::string &)>, std::string > (&string::validate_string, "string"));
            return trans;
        }

        static std::vector<std::pair<std::function<bool(const std::string &)>, std::string> > transfer = init();
        // For spec earlier than c++ 11

                /*        typedef bool func_check(const std::string &);
                typedef func_check * func_check_pointer;

                std::vector<std::pair<func_check_pointer, std::string> > init() {
                    std::vector<std::pair<func_check_pointer, std::string> > trans;// = new std::vector<std::pair<func_check_pointer, std::string> >();
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_printable, "char"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_bool, "bool"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_short, "short"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_ushort, "ushort"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_int, "int"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_uint, "uint"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_long, "long"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_ulong, "ulong"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_float, "float"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_double, "double"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_long_double, "long-double"));
                    trans.push_back(std::pair<func_check_pointer, std::string>(&string_validate_string, "string"));
                    return trans;
                }
                static std::vector<std::pair<func_check_pointer, std::string> > transfer = init();
         */
        static int get_entry_index(const std::string &value) {
            size_t len = transfer.size();
            int ret = -1;
            for (size_t i = 0; i < len; ++i) {
                if (transfer[i].second == value) {
                    ret = static_cast<int> (i);
                    break;
                }
            }
            return ret;
        }

        class entry {
        private:
            char* _vname;
            char* _type;
            char* _long;
            char* _short;
            char* _description;

            bool _option_validate(const char* type_, const char *default_) {
                assert(type_ != NULL);
                assert(default_ != NULL); 
                int index = get_entry_index(type_);
                return (index == -1 ? false : ((transfer[index].first)(default_)));
            }

            entry(char* vname_, const char* type_,
                const char* long_, const char* short_, const char* default_, const char *description_) {
                assert(vname_ != NULL);
                assert(type_ != NULL && (long_ != NULL || short_ != NULL));
                assert(default_ != NULL);
                assert(_option_validate(type_, default_));
                _vname = vname_;
                if (strcmp(type_, "string") == 0) {
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
                if (description_ != NULL)
                    _description = u::string::dup(description_);

                value(default_);
            }

	    static const char *type2string(char type_) {
                return "char";
	    }

	    static const char *type2string(bool type_) {
                return "bool";
	    }

	    static const char *type2string(short type_) {
                return "short";
	    }

	    static const char *type2string(unsigned short type_) {
                return "ushort";
	    }

	    static const char *type2string(int type_) {
                return "int";
	    }

	    static const char *type2string(unsigned int type_) {
                return "uint";
	    }

	    static const char *type2string(long type_) {
                return "long";
	    }

	    static const char *type2string(unsigned long type_) {
                return "ulong";
	    }

	    static const char *type2string(float type_) {
                return "float";
	    }

	    static const char *type2string(double type_) {
                return "double";
	    }

	    static const char *type2string(char * type_) {
                return "string";
	    }

        public:

            void release() {
                u::string::free(&_type);
                u::string::free(&_long);
                u::string::free(&_short);
            }

            template <typename T>
            static entry * create(T* vname_, const char* type_,
                    const char* long_, const char* short_, const char* default_, const char *description_ = NULL) {
                assert(type_ != NULL && (short_ != NULL || long_ != NULL));
                entry *ret = new entry(reinterpret_cast<char *> (vname_), type_, long_, short_, default_, description_);
                return ret;
            }

	    template <typename T>
            static entry * create(T* vname_, const char* long_, 
	            const char *short_, T default_, const char *description_=NULL) {
                return create<T>(vname_, type2string(*vname_), long_, short_, u::string::to_string<T>(default_), description_);
	    }

            ~entry() {
                release();
            }

            const char * type() const {
                return _type;
            }

            const char * get(const std::string &flag) const {
                assert(flag == "long" || flag == "short" || flag == "description" || flag == "type");
                const char *ret = NULL;
                if (flag == "long") {
                    ret = _long;
                } else if (flag == "short") {
                    ret = _short;
                } else if (flag == "description") {
                    ret = _description;
                } else if (flag == "type") {
                    ret = _type;
                }
                return ret;
            }

            char *value() {
                assert(_type != NULL);
                char *ret = NULL;
                if (strcmp(_type, "char") == 0) {
                    ret = new char(*_vname);
                } else if (strcmp(_type, "bool") == 0) {
                    bool* bool_ret = reinterpret_cast<bool*> (_vname);
                    if(*bool_ret)
                        ret = u::string::dup("true");
                    else
                        ret = u::string::dup("false");
                } else if (strcmp(_type, "short") == 0) {
                    short* short_ret = reinterpret_cast<short*> (_vname);
                    assert(u::string::to_string<short>(*short_ret, &ret));
                } else if (strcmp(_type, "ushort") == 0) {
                    unsigned short* ushort_ret = reinterpret_cast<unsigned short*> (_vname);
                    assert(u::string::to_string<unsigned short>(*ushort_ret, &ret));
                } else if (strcmp(_type, "int") == 0) {
                    int* int_ret = reinterpret_cast<int *> (_vname);
                    assert(u::string::to_string<int>(*int_ret, &ret));
                } else if (strcmp(_type, "uint") == 0) {
                    unsigned int* uint_ret = reinterpret_cast<unsigned int*> (_vname);
                    assert(u::string::to_string<unsigned int>(*uint_ret, &ret));
                } else if (strcmp(_type, "long") == 0) {
                    long* long_ret = reinterpret_cast<long *> (_vname);
                    assert(u::string::to_string<long>(*long_ret, &ret));
                } else if (strcmp(_type, "ulong") == 0) {
                    unsigned long* ulong_ret = reinterpret_cast<unsigned long*> (_vname);
                    assert(u::string::to_string<unsigned long>(*ulong_ret, &ret));
                } else if (strcmp(_type, "float") == 0) {
                    float* float_ret = reinterpret_cast<float*> (_vname);
                    assert(u::string::to_string<float>(*float_ret, &ret));
                } else if (strcmp(_type, "double") == 0) {
                    double* double_ret = reinterpret_cast<double *> (_vname);
                    assert(u::string::to_string<double>(*double_ret, &ret));
                } else if (strcmp(_type, "long-double") == 0) {
                    long double* long_double_ret = reinterpret_cast<long double*> (_vname);
                    assert(u::string::to_string<long double>(*long_double_ret, &ret));
                } else if (strcmp(_type, "string") == 0) { // may be memory leak here. i.e. more than twice calling value
                    char **vname = reinterpret_cast<char **> (_vname);
                    if (*vname != NULL)
                        ret = u::string::dup(*vname);
                } else {
                    assert(false);
                }
                return ret;
            }

            void value(const char* value_) {
                assert(value_ != NULL);

                if (strcmp(_type, "char") == 0) {
                    assert(u::string::validate_printable(value_));
                    *_vname = *value_;
                } else if (strcmp(_type, "bool") == 0) {
                    assert(strcmp(value_, "true") == 0 || strcmp(value_, "off") ||
                            strcmp(value_, "false") == 0 || strcmp(value_, "on"));

                    bool* ret = reinterpret_cast<bool*> (_vname);
                    if (strcmp(value_, "true") == 0 || strcmp(value_, "on") == 0) {
                        *ret = true;
                    } else {
                        *ret = false;
                    }
                } else if (strcmp(_type, "short") == 0) {
                    short* ret = reinterpret_cast<short*> (_vname);
                    assert(u::string::validate_type<short>(value_, ret));
                } else if (strcmp(_type, "ushort") == 0) {
                    unsigned short* ret = reinterpret_cast<unsigned short*> (_vname);
                    assert(u::string::validate_type<unsigned short>(value_, ret));
                } else if (strcmp(_type, "int") == 0) {
                    int* ret = reinterpret_cast<int *> (_vname);
                    assert(u::string::validate_type<int>(value_, ret));
                } else if (strcmp(_type, "uint") == 0) {
                    unsigned int* ret = reinterpret_cast<unsigned int*> (_vname);
                    assert(u::string::validate_type<unsigned int>(value_, ret));
                } else if (strcmp(_type, "long") == 0) {
                    long* ret = reinterpret_cast<long *> (_vname);
                    assert(u::string::validate_type<long>(value_, ret));
                } else if (strcmp(_type, "ulong") == 0) {
                    unsigned long* ret = reinterpret_cast<unsigned long*> (_vname);
                    assert(u::string::validate_type<unsigned long>(value_, ret));
                } else if (strcmp(_type, "float") == 0) {
                    float* ret = reinterpret_cast<float*> (_vname);
                    assert(u::string::validate_type<float>(value_, ret));
                } else if (strcmp(_type, "double") == 0) {
                    double* ret = reinterpret_cast<double *> (_vname);
                    assert(u::string::validate_type<double>(value_, ret));
                } else if (strcmp(_type, "long-double") == 0) {
                    long double* ret = reinterpret_cast<long double*> (_vname);
                    assert(u::string::validate_type<long double>(value_, ret));
                } else if (strcmp(_type, "string") == 0) { // may be memory leak here. i.e. more than twice calling value
                    char **vname = reinterpret_cast<char **> (_vname);
                    u::string::free(vname);
                    if (strcmp(value_, "$NULL") != 0)
                        *vname = u::string::dup(value_);
                } else {
                    assert(false);
                }
            }
        };

        static std::vector<std::pair<std::string, int> > find_options(const std::vector<entry*> &option, const std::string &key) {
            assert(key.size() >= 2);
            size_t len = option.size();
            std::string flag;
            std::vector<std::pair<std::string, int> > *pos = new std::vector<std::pair<std::string, int> >();
            if (key[1] == '-') {
                assert(key.size() >= 3);
                flag = "long";
                pos->push_back(std::pair<std::string, int>(key, -1));
            } else {
                for (size_t i = 1; i < key.size(); ++i) {
                    pos->push_back(std::pair<std::string, int>(std::string("-") + key[i], -1));
                }
                flag = "short";
            }
            for (size_t i = 0; i < pos->size(); ++i) {
                for (size_t k = 0; k < len; ++k) {
                    const char *value = option[k]->get(flag);
                    if (value != NULL && (*pos)[i].first == value) {
                        (*pos)[i].second = k;
                        break;
                    }
                }
            }
            return *pos;
        }

        static void help(const char* program, const std::vector<entry*> &option) {
            if (program != NULL) {
                std::cout << program << ": " << std::endl;
                size_t len = option.size();
                const char *tmp = NULL;
                for (size_t i = 0; i < len; ++i) {
                    tmp = option[i]->get("long");
                    if (tmp != NULL)
                        std::cout << "  " << tmp;
                    else
                        std::cout << "  <no-long>";
                    tmp = option[i]->get("short");
                    if (tmp != NULL)
                        std::cout << ", " << tmp;
                    else
                        std::cout << ", <no-short>";
                    std::cout << ", <" << option[i]->get("type") << ">";
                    tmp = option[i]->value();
                    if (tmp != NULL)
                        std::cout << ", " << tmp;
                    else
                        std::cout << ", <NULL>";
                    tmp = option[i]->get("description");
                    if (tmp != NULL)
                        std::cout << " : " << tmp;
                    else
                        std::cout << " : <no-description>";
                    std::cout << std::endl;
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
                if (argv[i][0] == '-') {
                    if (map.empty()) {
                        map = find_options(option, argv[i]);
                        size_t len = map.size();
                        bool reflush = true;
                        for (size_t n = 0; n < len; ++n) {
                            if (map[n].second == -1) {
                                marker->push_back(u::string::dup(map[n].first));
                            } else {
                                if (strcmp(option[map[n].second]->type(), "bool") == 0) {
                                    option[map[n].second]->value("on");
                                } else if (len == 1) { //single option
                                    reflush = false;
                                } else {
                                    assert(false); // rarely come here, unknow error
                                }
                            }
                        }
                        if (reflush)
                            map.clear();
                    } else { // negative number, i.e. -23.93
                        option[map[0].second]->value(argv[i]);
                        map.clear();
                    }
                } else {
                    if (!map.empty()) {
                        option[map[0].second]->value(argv[i]);
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
