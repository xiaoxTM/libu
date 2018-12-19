#ifndef __U_THREAD_HPP__
#define __U_THREAD_HPP__

#include "u-version.hpp"

/***
    u-thread-system.hpp thread pool function
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


/*-lpthread options for g++*/

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <functional>
#include "u-base.hpp"

namespace u {

    namespace ws {

        class worker {
        private:
            std::mutex *_mutex;
            std::condition_variable *_cv;
            int *_num;

            template <class Func, class... Args>
            void exec(Func func, Args &&... args) {
                {
                    std::unique_lock<std::mutex> lock(*_mutex);
                    _cv->wait(lock, [this] {
                        return (*_num) > 0;
                    });
                }
                --(*_num);
                _cv->notify_one();
                func(std::forward<Args>(args)...);
                {
                    std::lock_guard<std::mutex> lock(*_mutex);
                    ++(*_num);
                }
                _cv->notify_one();
            }

            template <class Instance, class Func, class... Args>
            void exec_memfun(Instance &instance, Func func, Args &&... args) {
                {
                    std::unique_lock<std::mutex> lock(*_mutex);
                    _cv->wait(lock, [this] {
                        return (*_num) > 0;
                    });
                }
                --(*_num);
                _cv->notify_one();
                (instance.*func)(std::forward<Args>(args)...);
                {
                    std::lock_guard<std::mutex> lock(*_mutex);
                    ++(*_num);
                }
                _cv->notify_one();
            }

        public:

            worker(int *num, std::mutex *mutex, std::condition_variable *cv) {
                _num = num;
                _mutex = mutex;
                _cv = cv;
            }

            template <class Func, class... Args>
            void run(bool async, Func& func, Args &&... args) {
                auto fun = std::bind(&worker::exec<Func, Args...>, this, func, args...);
                std::thread thread(fun, func, args...);
                if (async) {
                    thread.detach();
                } else {
                    thread.join();
                }
            }

            template <class Instance, class Func, class... Args>
            void run_memfun(bool async, Instance &instance, Func func, Args &&... args) {
                auto fun = std::bind(&worker::exec_memfun<Instance, Func, Args...>, this, instance, func, args...);
                std::thread thread(fun, instance, func, args...);
                if (async) {
                    thread.detach();
                } else {
                    thread.join();
                }
            }
        };

        class work_station {
        private:
            int _num;
            std::mutex _mutex;
            std::condition_variable _cv;
            std::vector<std::pair<worker*, bool> > _workers;

        public:

            work_station(int num) {
                assert(num > 0);
                _num = num;
                for (int i = 0; i < num; ++i) {
                    _workers.push_back(std::pair < worker*, bool>(new worker(&_num, &_mutex, &_cv), false));
                }
            }

            template <class Func, class...Args>
            int run(Func& func, Args &&...args) {
                int ret = 0;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cv.wait(lock, [this] {
                        return _num > 0;
                    });
                }
                for (int i = 0; i < _num; ++i) {
                    if (!_workers[i].second) {
                        _workers[i].second = true;
                        _cv.notify_one();
                        ret = i;
                        break;
                    }
                }
                _workers[ret].first->run(true, func, args...);
                return ret;
            }

            template <class Instance, class Func, class...Args>
            int run_memfun(Instance &instance, Func func, Args &&...args) {
                int ret = 0;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cv.wait(lock, [this] {
                        return _num > 0;
                    });
                }
                for (int i = 0; i < _num; ++i) {
                    if (!_workers[i].second) {
                        _workers[i].second = true;
                        _cv.notify_one();
                        ret = i;
                        break;
                    }
                }
                _workers[ret].first->run_memfun(true, instance, func, args...);
                return ret;
            }
        };
    }
}

#endif
