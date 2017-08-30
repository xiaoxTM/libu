#ifndef __U_KBHIT_HPP__
#define __U_KBHIT_HPP__

#warning "compiling with this file, do not forget to add option: -lpthread"

/***
    keyboard.hpp keyboard event collection
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

    NOTE: for better performance, this library needs pthread, please compile
    your program with library options:
                       -lpthread
***/

#if defined(_WIN64) || defined(_WIN32)
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#endif

namespace u {
    namespace event {

    class keyboard
    {
    public:
#if !defined(_WIN64_) && !defined(_WIN32_)

        static struct termios _initial_settings;
        static struct termios _new_settings;
        static int _peek_character;
        static int get() {
            keyboard::open();
            int key = keyboard::getc();
            keyboard::close();
            return key;
        }
        static void open() {
            tcgetattr(0,&_initial_settings);
            _new_settings = _initial_settings;
            _new_settings.c_lflag &= ~ICANON;
            _new_settings.c_lflag &= ~ECHO;
            _new_settings.c_lflag &= ~ISIG;
            _new_settings.c_cc[VMIN] = 1;
            _new_settings.c_cc[VTIME] = 0;
            tcsetattr(0, TCSANOW, &_new_settings);
            _peek_character=-1;
        }

        static void close() {
            tcsetattr(0, TCSANOW, &_initial_settings);
        }

        static int hit() {
            unsigned char ch;
            int nread;

            if (_peek_character != -1) return 1;
            _new_settings.c_cc[VMIN]=0;
            tcsetattr(0, TCSANOW, &_new_settings);
            nread = read(0,&ch,1);
            _new_settings.c_cc[VMIN]=1;
            tcsetattr(0, TCSANOW, &_new_settings);

            if (nread == 1)
            {
                _peek_character = ch;
                return 1;
            }
            return 0;
        }

        static int getc() {
            char ch;

            if (_peek_character != -1)
            {
                ch = _peek_character;
                _peek_character = -1;
            } else read(0,&ch,1);

            return ch;
        }
#else
    int keyboard::get()
    {
        return getch();
    }
#endif
    private:
        keyboard(){};
        ~keyboard(){};
    };

    struct termios keyboard::_initial_settings;
    struct termios keyboard::_new_settings;
    int keyboard::_peek_character;
    }

    /**!
     * @Function: wait for special key been pressed, or wait for @milliseconds time
     * @Param:
     *          -- key:          the key to be waited to press, -1 for no key pressed
     *          -- milliseconds: milli second to wait. if 0, wait until @key been pressed
     *          -- hint:         hint message to user if milliseconds equals zero and not empty
     * @Return: the key been pressed
     **/
    template <int key=-1>
    int wait_key(int milliseconds, const std::string &hint = std::string()) {
        int press_key = -1;
        if (milliseconds == 0 && ! hint.empty())
            std::cout << hint << std::flush;
        std::thread key_thread([&](){
                press_key = static_cast<int>(event::keyboard::get());
                });
        key_thread.detach();
        if (milliseconds > 0) {
            while((press_key == -1 || press_key != key) && (--milliseconds) > 0)
                std::this_thread::sleep_for(std::chrono::microseconds(100));
        } else {
            while((press_key == -1 || press_key != key))
                std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        return press_key;
    }
}

#endif
