#ifndef __U_CHECKER_HPP__
#define __U_CHECKER_HPP__

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

#include "u-string.hpp"
#include "u-timer.hpp"
#include <iostream>

namespace u {
    template <typename static_members>
    struct checker_static_holder
    {
        static float _succeed;
        static float _failure;
        static unsigned char _flag;
        // flag representation:
        // _flag: 00000000
        //             |||- current time
        //             ||- time used (runned time)
        //             |- initialization
    };

    template<typename static_members>
    float checker_static_holder<static_members>::_succeed;

    template<typename static_members>
    float checker_static_holder<static_members>::_failure;

    template<typename static_members>
    unsigned char checker_static_holder<static_members>::_flag;

    class checker : public checker_static_holder<void> {
    private:
        static void init(bool show_current_time=true, bool show_run_time=false) {
            _succeed = 0;
            _failure = 0;
            _flag = 0;
            _flag = 0x08;
            current_time(show_current_time);
            run_time(show_run_time);
        }
    public:

        static void current_time(bool show) {
            if (show) {
                _flag |= 0x01;
            } else {
                _flag &= 0xFE;
            }
        }

        static bool current_time() {
            return ((_flag & 0x01) == 0x01);
        }

        static void run_time(bool show) {
            if (show) {
                _flag |= 0x02;
            } else {
                _flag &= 0xFD;
            }
        }

        static bool run_time() {
            return ((_flag & 0x02) == 0x02);
        }

        static bool initialized() {
            return ((_flag & 0x08) == 0x08);
        }

        static bool begin(bool express) {
            if (!initialized()) {
                init();
            }
            if (current_time()) {
                std::cout << "[" <<u::timer::now() << " C]" << std::flush;
            }
            if (express) {
                _succeed += 1;
            } else {
                _failure += 1;
            }
            return express;
        }

        static void summary() {
            std::cout << _succeed + _failure << u::string::styled_string(" Checked.", u::string::fore_blue) << " #" << _succeed << u::string::styled_string(" Succeed", u::string::fore_green) << ", #" << _failure << u::string::styled_string(" Failed", u::string::fore_red) << std::endl;
        }

        static std::ostream &end(bool express, const std::string &expr, const std::string &assert_function, const std::string &file, const int line, bool newline=true) {
            if (run_time()) {
                std::cout << " +"<< u::timer::end() << "(s)";
            }
            std::cout << " `" << u::string::styled_string(expr, u::string::fore_blue) << "` #" << assert_function << " @" << file  << "(" << line << ")# ==> ";
            if (express) {
                std::cout << u::string::styled_string("SUCCEED", u::string::fore_green) << std::flush;
            } else {
                std::cout << u::string::styled_string("FAILED", u::string::fore_red) << std::flush;
            }
            if (newline) {
                std::cout << std::endl;
            }
            return std::cout;
        }

#ifndef u_check
#define u_check(expr, newline)	\
  {u::timer::begin();(u::checker::begin((expr))			\
   ? u::checker::end(true, #expr, __ASSERT_FUNCTION, __FILE__, __LINE__, newline) \
   : u::checker::end(false, #expr, __ASSERT_FUNCTION, __FILE__, __LINE__, newline));}
#endif

#ifndef u_ncheck
#define u_ncheck(expr)	\
  {u::timer::begin();(u::checker::begin((expr))			\
   ? u::checker::end(true, #expr, __ASSERT_FUNCTION, __FILE__, __LINE__, true) \
   : u::checker::end(false, #expr, __ASSERT_FUNCTION, __FILE__, __LINE__, true));}
#endif

    };
}
#endif
