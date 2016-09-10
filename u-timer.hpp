#ifndef __U_TIMER_HPP__
#define __U_TIMER_HPP__

/***
  u-timer.hpp base functions for libu
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

#include <ctime>
#include <stack>

namespace u {

    template <class static_member>
    struct timer_static_holder
    {
        static std::stack<clock_t> timers;
    };

    template <class static_member>
    std::stack<clock_t> timer_static_holder<static_member>::timers;

    class timer : public timer_static_holder<void> {
    public:

        /**
         * time the run-time of class instance @instance function @fun
         * NOTE: all parameters of function @fun must be specified,
         *       including the <emph>default</emph> ones
         */
        template <class Instance, class Fun>
        static double mrun(Instance &instance, Fun fun) {
            clock_t start = clock();
            (instance.*fun)();
            clock_t diff = clock() - start;
            return (static_cast<double> (diff) / CLOCKS_PER_SEC);
        }

        template <class Fun>
        static double run(Fun &fun) {
            clock_t start = clock();
            fun();
            clock_t diff = clock() - start;
            return (static_cast<double> (diff) / CLOCKS_PER_SEC);
        }

        template <class Instance, class Fun, class ...Args>
        static double mrun(Instance &instance, Fun fun, Args &&... args) {
            clock_t start = clock();
            (instance.*fun)(std::forward<Args>(args)...);
            clock_t diff = clock() - start;
            return (static_cast<double> (diff) / CLOCKS_PER_SEC);
        }

        template <class Fun, class...Args>
        static double run(Fun &fun, Args &&... args) {
            clock_t start = clock();
            fun(std::forward<Args>(args)...);
            clock_t diff = clock() - start;
            return (static_cast<double> (diff) / CLOCKS_PER_SEC);
        }

        static void begin() {
            timers.push(clock());
        }

        static double end() {
            clock_t t = clock() - timers.top();
            timers.pop();
            return (static_cast<double> (t) / CLOCKS_PER_SEC);
        }

        static void clean() {
            while (!timers.empty()) {
                timers.pop();
            }
        }
    };
}

#endif
