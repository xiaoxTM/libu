#ifndef __U_BASE_HPP__
#define __U_BASE_HPP__

/***
  u-base.hpp base functions for libu
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

#include <cstdarg>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <limits>
#include <memory>

namespace u {

#if defined(_WIN64) || defined(_WIN32)
#define SYSTEM_PATH_SEPARATOR "\\"
#else
#define SYSTEM_PATH_SEPARATOR "/"
#endif

    /*represent for 'None'*/
    const static unsigned char N = 0x00;

    /*represent for 'Directory', and/or 'Default'*/
    const static unsigned char D = 0x01;

    /*represent for 'Suffix'*/
    const static unsigned char S = 0x01;

    /*represent for 'File'*/
    const static unsigned char F = 0x02;

    /*represent for 'Terminal', and/or 'Top'*/
    const static unsigned char T = 0x04;

    /*represent for 'Prefix'*/
    const static unsigned char P = 0x04;

    /*represent for 'Open'*/
    const static unsigned char O = 0x08;

    /*represent for 'Left'*/
    const static unsigned char L = 0x00;

    /*represent for 'Right'*/
    const static unsigned char R = 0x01;

    /*represent for flush flag of stream*/
    const static unsigned char FLUSH = 0x10;

    const static char * KEY_ESC = "ESC";
    const static char * KEY_BACKSPACE = "BACKSPACE";
    const static char * KEY_HOME = "HOME";
    const static char * KEY_TAB = "TAB";
    const static char * KEY_SPACE = "SPACE";
    const static char * KEY_ENTER = "ENTER";
    const static char * KEY_DEL = "DELETE";
    const static char * KEY_SHIFT_L = "SHIFT-L";
    const static char * KEY_SHIFT_R = "SHIFT-R";
    const static char * KEY_ALT_L = "ALT-L";
    const static char * KEY_ALT_R = "ALT-R";
    const static char * KEY_CTL_L = "ALT-L";
    const static char * KEY_CTL_R = "ALT-R";
    const static char * KEY_WIN_L = "WIN-L";
    const static char * KEY_WIN_R = "WIN-R";

    template <typename T>
    static T max() {
        return std::numeric_limits<T>::max();
    }

    template <typename T>
    static T min() {
        return std::numeric_limits<T>::min();
    }

    static std::map<const std::string, int> make_key_map() {
        std::map<const std::string, int> ret;
        ret[KEY_BACKSPACE] = 8;
        ret[KEY_TAB] = 9;
        ret[KEY_HOME] = 11;
        ret[KEY_ENTER] = 10;
        ret[KEY_ESC] = 27;
        ret[KEY_SPACE] = 32;
        ret[KEY_SHIFT_L] = 65505;
        ret[KEY_SHIFT_R] = 65506;
        ret[KEY_CTL_L] = 65507;
        ret[KEY_CTL_R] = 65508;
        ret[KEY_ALT_L] = 65513;
        ret[KEY_ALT_R] = 65514;
        ret[KEY_WIN_L] = 65515;
        ret[KEY_WIN_R] = 65516;
        ret["!"] = 33;
        ret["\""] = 34;
        ret["#"] = 35;
        ret["$"] = 36;
        ret["%"] = 37;
        ret["&"] = 38;
        ret["'"] = 39;
        ret["("] = 40;
        ret[")"] = 41;
        ret["*"] = 42;
        ret["+"] = 43;
        ret[","] = 44;
        ret["-"] = 45;
        ret["."] = 46;
        ret["/"] = 47;
        ret["0"] = 48;
        ret["1"] = 49;
        ret["2"] = 50;
        ret["3"] = 51;
        ret["4"] = 52;
        ret["5"] = 53;
        ret["6"] = 54;
        ret["["] = 91;
        ret["7"] = 55;
        ret["\\"] = 92;
        ret["8"] = 56;
        ret["]"] = 93;
        ret["9"] = 57;
        ret["^"] = 94;
        ret[":"] = 58;
        ret["_"] = 95;
        ret[";"] = 59;
        ret["`"] = 96;
        ret["<"] = 60;
        ret["{"] = 123;
        ret["="] = 61;
        ret["|"] = 124;
        ret[">"] = 62;
        ret["}"] = 125;
        ret["?"] = 63;
        ret["~"] = 126;
        ret["@"] = 64;
        ret[KEY_DEL] = 127;
        ret["A"] = 65;
        ret["a"] = 97;
        ret["B"] = 66;
        ret["b"] = 98;
        ret["C"] = 67;
        ret["c"] = 99;
        ret["D"] = 68;
        ret["d"] = 100;
        ret["E"] = 69;
        ret["e"] = 101;
        ret["F"] = 70;
        ret["f"] = 102;
        ret["G"] = 71;
        ret["g"] = 103;
        ret["H"] = 72;
        ret["h"] = 104;
        ret["I"] = 73;
        ret["i"] = 105;
        ret["J"] = 74;
        ret["j"] = 106;
        ret["K"] = 75;
        ret["k"] = 107;
        ret["L"] = 76;
        ret["l"] = 108;
        ret["M"] = 77;
        ret["m"] = 109;
        ret["N"] = 78;
        ret["n"] = 110;
        ret["O"] = 79;
        ret["o"] = 111;
        ret["P"] = 80;
        ret["p"] = 112;
        ret["Q"] = 81;
        ret["q"] = 113;
        ret["R"] = 82;
        ret["r"] = 114;
        ret["S"] = 83;
        ret["s"] = 115;
        ret["T"] = 84;
        ret["t"] = 116;
        ret["U"] = 85;
        ret["u"] = 117;
        ret["V"] = 86;
        ret["v"] = 118;
        ret["W"] = 87;
        ret["w"] = 119;
        ret["X"] = 88;
        ret["x"] = 120;
        ret["Y"] = 89;
        ret["y"] = 121;
        ret["Z"] = 90;
        ret["z"] = 122;
        return ret;
    }

    static std::map<const std::string, int> _key_map;

    static int key(const std::string &ascii) {
        if (_key_map.empty()) {
            _key_map = make_key_map();
        }
        int key_value = max<int>();
        std::map<const std::string, int>::iterator it = _key_map.find(ascii);
        if (it != _key_map.end()) {
            key_value = it->second;
        }
        return key_value;
    }

    /**
     ** @Function  -- convert a given data type to string
     ** @Parameters:
     **            -- value: data type to be converted
     **            -- ret: a storage to store coverted string if ret is not null
     ** @Return    -- true if no error ocurred, otherwise false returned
     ***/
    static char * va_format(const char *format, va_list arg) {
        char * ret = NULL;
        std::unique_ptr<char [] > formatted;
        int buffer_size = 1024;
        int needed = 0;
        if (format != NULL) {
            while (true) {
                formatted.reset(new char[buffer_size]);
                needed = vsnprintf(&formatted[0], buffer_size, format, arg);
                if (needed <= 0 || needed > buffer_size) {
                    buffer_size += std::abs(needed - buffer_size + 1);
                } else {
                    break;
                }
            }
            if (needed < buffer_size) {
                ret = new char[needed + 1];
                memcpy(ret, formatted.get(), needed + 1);
                formatted.release();
            }
        }
        return ret;
    }

    static char * format(const char *format, ...) {
        char * ret = NULL;
        if (format != NULL) {
            va_list arg_list;
            va_start(arg_list, format);
            ret = u::va_format(format, arg_list);
            va_end(arg_list);
        }
        return ret;
    }
}

#endif
