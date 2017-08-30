#ifndef __U_TIMER_HPP__
#define __U_TIMER_HPP__

#include "u-version.hpp"

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
        static double run_memfun(Instance &instance, Fun fun) {
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
        static double run_memfun(Instance &instance, Fun fun, Args &&... args) {
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

        static std::string now(const std::string &fmt="%a %F %H:%M:%S") {
            char now_time[100];
            std::time_t t = std::time(nullptr);
            std::strftime(now_time, sizeof(now_time), fmt.c_str(), std::localtime(&t));
            return std::string(now_time);
        }

        static std::string today() {
            return now("%F");
        }

        static std::string week(bool fullname=true) {
            std::string fmt = "%A";
            if (! fullname) {
                fmt = "%a";
            }
            return now(fmt);
        }
    };
}

#endif
