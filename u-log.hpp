#ifndef __U_LOG_HPP__
#define __U_LOG_HPP__

/***
  u-log.hpp logger function collection
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

#include "u-string.hpp"
#include "u-path.hpp"

#include <cmath>

namespace u {

    template <typename static_members>
    struct log_static_holder
    {
        static unsigned short _flag;
        static char * _filename;
        static std::ofstream _ofs;
        static char * _prompt;
        static int _indent;
        static char _fill;
    };

    template<typename static_members>
    unsigned short log_static_holder<static_members>::_flag;

    template<typename static_members>
    char * log_static_holder<static_members>::_filename;

    template<typename static_members>
    std::ofstream log_static_holder<static_members>::_ofs;

    template<typename static_members>
    char * log_static_holder<static_members>::_prompt;

    template<typename static_members>
    int log_static_holder<static_members>::_indent;

    template<typename static_members>
    char log_static_holder<static_members>::_fill;

    /**
     *                    |-- FLUSH flag
     * _flag: 00000000 00000000
     *        |-------     ||||_ Default flag
     *        |   |        |||__ File flag
     *        |   |        ||___ Term flag
     *        |   |        |____ Log open flag
     *        |   |_ Print flag
     *        |_ Debug flag
    **/
    class log : public log_static_holder<void> {
    private:
        static bool masked(unsigned short flag) {
	    return (((flag & _flag) & 0xFF00) == 0);
        }

        static void init() {
            _flag = 0x0000;
            _filename = NULL;
            _prompt = NULL;
            _indent = 0;
            _fill = ' ';
        }

        log() {
        };

    public:

        /*
         * @function open: open log system for logging
         * @params
         *   @flag for controlling whether log or not, the meaning of each bit is
         *         explained as follow:
         *                    |-- FLUSH flag: enable flush stream after each printing
         * _flag: 00000000 00000000
         *        |-------     ||||_ Default flag
         *        |   |        |||__ File flag: logging to file @_filename instead of terminal
         *        |   |        ||___ Term flag: print message to terminal
         *        |   |        |____ Log open flag: record whether log system is opened or not
         *        |   |_ Print flag: print level flag, that is, message will be printed only if corresponding bit is set
         *        |                  we have 7 bits (levels) different print model. consider each bit as virtual terminal
         *        |_ Debug flag
         *   For example, if we want to debug, then set the most left bit to be 1.
         *   @indent for printing @indent @fill before printing message
         *           for example, if @indent = 4, and @fill = '*', it prints "***hello world" instead of "hello world".
         *   @fill see @indent
         *   @filename filename to logging message if @flag File bit is set
         *   @mode mode to open @filename
         */
        static bool open(unsigned short flag = (u::D | 0x7F00), int indent = 0, char fill = ' ', const std::string &filename = std::string(), std::ios_base::openmode mode = std::ofstream::out | std::ofstream::app) {
            init();
            bool ret = true;
            if ((flag & u::D) == u::D) {
                _flag = ((u::T | u::FLUSH) | (flag & 0xFF00));
            } else {
                _flag = flag;
                if ((flag & (u::F | u::T)) == (u::F | u::T) || (flag & u::F) == u::F) {
                    if (filename.empty()) {
                        ret = false;
                    } else {
                        _filename = u::string::dup(filename);
                        _ofs.open(_filename, mode);
                    }
                }
            }
            if (ret) {
                _flag |= u::O;
            }
            _indent = indent;
            _fill = fill;
            return ret;
        }

        /*
         * @function reopen: reopen log system. It only changes open bit of @_flag
         */
        static void reopen() {
            if (! opened()) {
                _flag |= u::O;
            }
        }

        /*
         * @function close: set open bit to 0 in @_flag
         */
        static void close() {
            if (opened()) {
                _flag &= 0xF7;
            }
        }

        /*
         * @function term: print message to terminal no matter log system is opened or not.
         * @params
         *   @bspace print @bspace (>0) + @_indent @_fill before printing message
         *   @aspace adjust @_indent when @aspace less than 0 (used for restore state)
         *   @format formatted message to print
         */
        static void term(int bspace, int aspace, const char* format, ...) {
            if (format != NULL) {
                char * msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                msg = va_format(format, arg_list);
                va_end(arg_list);
                std::string message;
                if (bspace >= 0) {
                    _indent += bspace;
                    if (_indent <= 0) {
                        _indent = 0;
                    } else {
                        message.append(_indent, _fill);
                    }
                }
                message.append(msg);
                if (aspace < 0) {
                    _indent += aspace;
                    if (_indent <= 0) {
                        _indent = 0;
                    }
                }
                std::cout << message << std::flush;
            }
        }

        /*
         * @function save: save message to file no matter which kind of model log system is opened.
         *                 however, it is controlled by @level, just like @_flag print flags
         * @params
         *   @level level for logging
         *   @filename file @filename to logging message
         *   @mode mode for open file @filename
         *   @format formatted message to log
         */
        template <unsigned short level=0x7F00>
        static bool save(const std::string &filename, std::ios_base::openmode mode, const char* format, ...) {
            bool ret = false;
            if (format != NULL && !masked(level)) {
                std::ofstream ofs(filename);
                if (!ofs.fail()) {
                    char * msg = NULL;
                    va_list arg_list;
                    va_start(arg_list, format);
                    msg = va_format(format, arg_list);
                    va_end(arg_list);
                    ofs << msg << std::flush;
                    ret = true;
                    ofs.close();
                }
            }
            return ret;
        }

        template <size_t num = 10, bool brief = true, unsigned short level=0x7F00>
        static void line(size_t ith, size_t total, const char * format, ...) {
	    if (masked(level))
	        return;
            assert(num != 0 && total != 0);
	    if (format != NULL) {
                char * msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                msg = va_format(format, arg_list);
                va_end(arg_list);

                ++ith;

                double size = total / static_cast<double> (num);
                size_t div = static_cast<size_t> (std::ceil(ith / size) - 1);
                size_t step = std::ceil(num / static_cast<double> (total)) - 1;
                if (step == 0)
                    step = 1;
                static size_t len = 0;

                if (_prompt != NULL && len != num) {
                    u::string::free(& _prompt);
                }
                if (_prompt == NULL) {
                    _prompt = new char[num + 1];
                    len = num;
                    std::fill_n(_prompt, len, '+');
                    _prompt[len] = '\0';
                }

                if (div >= len) {
                    div = len - 1;
                }
                if (div > 0)
                    std::fill_n(_prompt, div, '+');
                if (_prompt[div] == 'x' || ith == total) {
                    _prompt[div] = '+';
                } else {
                    _prompt[div] = 'x';
                }
                _prompt[div + 1] = '\0';
                char *message = NULL;
                char *_format = NULL;
                if (brief) {
                    _format = u::format("%%s [%%-%us, %%u\%]\r", num);
                    message = u::format(_format, msg, _prompt, (ith * 100 / total));
                } else {
                    _format = u::format("%%s [%%-%us, %%u / %%u, %%u\%]\r", num);
                    message = u::format(_format, msg, _prompt, ith, total, (ith * 100 / total));
                }

                u::log::term(0, 0, "%s", message);
                u::string::free(&_format);
                u::string::free(&message);
	    }
        }

        ~log() {
            u::string::free(& _filename);
            u::string::free(& _prompt);
            if (_ofs.is_open())
                _ofs.close();
        }

        template <typename T = const std::string &>
        static void print(T msg, unsigned short flag = (u::D | 0x7F00)) {
            if (!opened() || masked(flag))
                return;
            if ((flag & u::D) == u::D) {
                if ((_flag & (u::F | u::T)) == (u::F | u::T)) {
                    _ofs << msg;
                    std::cout << msg;
                    if ((_flag & u::FLUSH) == u::FLUSH)
                        std::cout << std::flush;
                } else if ((_flag & u::F) == u::F && _ofs.is_open()) {
                    _ofs << msg;
                    if ((_flag & u::FLUSH) == u::FLUSH) {
                       _ofs << std::flush;
                    }
                } else if ((_flag & u::T) == u::T) {
                    std::cout << msg;
                    if ((_flag & u::FLUSH) == u::FLUSH) {
                        std::cout << std::flush;
                    }
                }
            } else if ((flag & (u::F | u::T)) == (u::T | u::F)) {
                if (_ofs.is_open())
                    _ofs << msg;
                std::cout << msg;
                if ((flag & u::FLUSH) == u::FLUSH)
                    std::cout << std::flush;
            } else if ((flag & u::F) == u::F && _ofs.is_open()) {
                _ofs << msg;
            } else if ((flag & u::T) == u::T) {
                std::cout << msg;
                if ((flag & u::FLUSH) == u::FLUSH)
                    std::cout << std::flush;
            }
        }

        template <unsigned short flag = (u::D | 0x7F00)>
        static void echo(const char *format, ...) {
            if (opened() && !masked(flag) && format != NULL) {
                char * msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                msg = u::va_format(format, arg_list);
                va_end(arg_list);
                print(msg, flag);
            }
        }

	template <unsigned short flag = (u::D | 0x7F00)>
        static void indent(int bspace, int aspace, const char *format, ...) {
            if (opened() && !masked(flag) && format != NULL) {
                char *format_msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                format_msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::string msg;
                if (bspace >= 0) {
                    _indent += bspace;
                    if (_indent < 0) {
                        _indent = 0;
                    } else {
                        msg.append(_indent, _fill);
                    }
                }
                msg.append(format_msg);
                msg.append("\n");
                print(msg.c_str(), flag);
                if (aspace < 0) {
                    _indent += aspace;
                    if (_indent < 0) {
                        _indent = 0;
                    }
                }
            }
        }

	template <unsigned short flag = (u::D | 0x7F00)>
        static void indent(int space, const char *format, ...) {
            if (opened() && !masked(flag) && format != NULL) {
                char *format_msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                format_msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::string msg;
                if (space >= 0) {
                    _indent += space;
                    if (_indent < 0) {
                        _indent = 0;
                    } else {
                        msg.append(_indent, _fill);
                    }
                }
                msg.append(format_msg);
                msg.append("\n");
                print(msg.c_str(), flag);
                if (space < 0) {
                    _indent += space;
                    if (_indent < 0) {
                        _indent = 0;
                    }
                }
            }
        }

	template <unsigned short flag = (u::D | 0x7F00)>
        static void indent(const char *format, ...) {
            if (opened() && !masked(flag) && format != NULL) {
                char *format_msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                format_msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::string msg;
                msg.append(_indent, _fill);
                msg.append(format_msg);
                msg.append("\n");
                print(msg.c_str(), flag);
            }
        }

	template <unsigned short flag = (u::D | 0x7F00)>
        static void indent(int spaces) {
            if (opened() && !masked(flag)) {
                _indent += spaces;
                if (_indent <= 0) {
                    _indent = 0;
                }
            }
        }

        static void debug(const char *format, ...) {
            if (opened() && !masked(0x8000) && format != NULL) {
                char *format_msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                format_msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::string msg;
                msg.append(_indent, _fill);
                msg.append(format_msg);
                msg.append("\n");
                print(msg.c_str(), (u::D|0x8000));
            }
        }

        static void debug(int bspace, int aspace, const char *format, ...) {
            if (opened() && !masked(0x8000) && format != NULL) {
                char *format_msg = NULL;
                va_list arg_list;
                va_start(arg_list, format);
                format_msg = u::va_format(format, arg_list);
                va_end(arg_list);
                std::string msg;
                if (bspace >= 0) {
                    _indent += bspace;
                    if (_indent < 0) {
                        _indent = 0;
                    } else {
                        msg.append(_indent, _fill);
                    }
                }
                msg.append(format_msg);
                msg.append("\n");
                print(msg.c_str(), (u::D|0x8000));
                if (aspace < 0) {
                    _indent += aspace;
                    if (_indent < 0) {
                        _indent = 0;
                    }
                }
            }
        }

        static bool opened() {
            return ((_flag & u::O) == u::O);
        }

        template <unsigned short flag = (u::D | 0x7F00)>
        static void message(const char* format, ...) {
            if (opened() && format != NULL) {
                std::string msg("...... message ......\n");
                va_list arg;
                va_start(arg, format);
                msg.append(va_format(format, arg));
                va_end(arg);
                msg.append("\n...... **end** ......\n");
                print(msg, flag);
            }
        }

        template <unsigned short flag = (u::D | 0x7F00)>
        static void warning(const char* format, ...) {
            if (opened() && format != NULL) {
                std::string msg("...... warning ......\n");
                va_list arg;
                va_start(arg, format);
                msg.append(va_format(format, arg));
                va_end(arg);
                msg.append("\n...... **end** ......\n");
                print(msg, flag);
            }
        }

        template <unsigned short flag = (u::D | 0xFF00)>
        static void error(const char* format, ...) {
            if (opened() && format != NULL) {
                std::string msg("****** error ******\n");
                va_list arg;
                va_start(arg, format);
                msg.append(va_format(format, arg));
                va_end(arg);
                msg.append("\n****** **end** ******\n");
                print(msg, flag);
            }
        }

        template <unsigned short flag = (u::D | 0xFF00)>
        static void fatal(const char*format, ...) {
            if (opened() && format != NULL) {
                std::string msg("###### fatal ######\n");
                va_list arg;
                va_start(arg, format);
                msg.append(va_format(format, arg));
                va_end(arg);
                msg.append("\n###### **end** ######\n");
                print(msg, flag);
            }
        }

#define u_fun_enter(a,b) {u::log::debug(a, b, "[%s:%d `%s`] enter", __FILE__, __LINE__, __FUNCTION__);}
#define u_fun_exit(a,b) {u::log::debug(a, b, "[%s:%d `%s`] exit", __FILE__, __LINE__, __FUNCTION__);}


/*
 * @function u_assert: assert whether @expr is true or not
 * @params
 *   @expr true/false expression, if false, print message @msg
 *       and then abort program
 *
 **/
#ifndef u_assert
#define u_assert(expr,msg)	\
  ((expr)			\
   ? __ASSERT_VOID_CAST (0)	\
   : __assert_fail (u::format("%s [%s]", #expr, msg), __FILE__, __LINE__, __ASSERT_FUNCTION))
#endif

/*
 * @function u_return: print exiting function message, then return
 * @params
 *   @a @b for <link>u_fun_exit</link> function
 *   @value for return
 */
#ifndef u_return
#define u_return(a,b,value)     \
  u_fun_exit(a,b);              \
  return (value)
#endif

/*
 * @function u_return_void: print exiting function message, then return no value
 * @params
 *   @a @b for <link>u_fun_exit</link> function
 */
#ifndef u_return_void
#define u_return_void(a,b)      \
  u_fun_exit(a,b);              \
  return;
#endif

/*
 * @function u_return_if: return no value if @expr is true, else do nothing
 * @params
 *   @a @b for <link>u_fun_exit</link>
 *   @expr for controlling whether return or not
 */
#ifndef u_return_if
#define u_return_if(a,b,expr)   \
  if(expr) {                    \
    u_fun_exit(a,b);            \
    return;                     \
  }
#endif

/*
 * @function u_return_value_if: return value @value if @expr is true, else do nothing
 * @params
 *   @a @b for <link>u_fun_exit</link>
 *   @expr for controlling whether return or not
 *   @value to be returned
 */
#ifndef u_return_value_if
#define u_return_value_if(a,b,expr,value) \
  if(expr) {                              \
    u_fun_exit(a,b)                       \
    return (value);                       \
  }
#endif

    };
}


#endif
