#ifndef __U_CACHE_HPP__
#define __U_CACHE_HPP__

#include "u-version.hpp"

#include "u-io.hpp"
#include "u-path.hpp"
#include "u-keyboard.hpp"

namespace u {

    enum ion_mode_e {
        ION_LOAD,
        ION_SAVE
    };

    template <typename static_members>
    struct cache_static_holder
    {
        static unsigned char _flag;
        static std::map<const std::string, std::vector<std::string> > _ignores[2];
        static std::map<const std::string, std::vector<std::string> > _checks[2];
    };

    template <typename static_members>
    unsigned char cache_static_holder<static_members>::_flag;

    template <typename static_members>
    std::map<const std::string, std::vector<std::string> > chace_static_holder<static_members>::_ignores[2];

    template <typename static_members>
    std::map<const std::string, std::vector<std::string> > chace_static_holder<static_members>::_checks[2];

    /*
     *                                    |------ save directory ion flag:
     *                                    |           if set, saving cache files to the same directory will be in interactive mode
     *                                    | |---- save ion flag
     * _flag: unsigned char =(8 bit) 0000 0000
     *                                     | |___ load ion flag
     *                                     |_____ load directory ion flag:
     *                                                if set, loading cache files to the same directory will be in interactive mode
     */
    class cache : public cache_static_holder<void>
    {
        public:
        /*
         *@function ion: set interactive (or not) when loading from / saving to cache file
         *               if @*ion is true, when loading / saving, ask to read or not
         *               if @*ion is false, loading / saving without asking
         */
        static void ion_load(bool flag)
        {
            _flag = (_flag & 0xFE); // clear load ion flag bit
            if (flag) {
                _flag |= 0x01; // set load ion flag bit
            }
        }

        inline static bool ion_load() const
        {
            return ((_flag & 0x01) == 0x01);
        }

        static void ion_load_dirs(bool flag)
        {
            _flag = (_flag & 0xFB);
            if (flag) {
                _flag |= 0x04;
            }
        }

        inline static bool ion_load_dirs() const
        {
            return ((_flag & 0x04) == 0x04);
        }

        static void ion_save(bool flag)
        {
            _flag = (_flag & 0xFD); // clear save ion flag bit
            if (flag) {
                _flag |= 0x02; // set save ion flag bit
            }
        }

        inline static bool ion_save() const
        {
            return ((_flag & 0x02) == 0x02);
        }

        static void ion_save_dirs(bool flag)
        {
            _flag = (_flag & 0xF7);
            if (flag) {
                _flag |= 0x08;
            }
        }

        inline static bool ion_save_dirs() const
        {
            return ((_flag & 0x08) == 0x08);
        }

        static void ion(bool lion, bool sion, bool dlion, bool dsion)
        {
            ion_load(lion);
            ion_save(sion);
            ion_load_dirs(dlion);
            ion_save_dirs(dsion);
        }

        static bool ion(ion_mode_e mode)
        {
            bool ret = ion_load();
            if (mode == ion_mode_e::ION_SAVE) {
                ret = ion_save();
            }
            return ret;
        }

        static bool ion_dirs(ion_mode_e mode)
        {
            bool ret = ion_load_dirs();
            if (mode == ion_mode_e::ION_SAVE) {
                ret = ion_save_dirs();
            }
            return ret;
        }

        inline static bool ion_include_dir(const std::map<const std::string, std::vector<std::string> > &storage, const std::string &value)
        {
            return (storage.find(value) != storage.end());
        }

        static bool ion_include_file(const std::map<const std::string, std::vector<std:s:tring> > &storage, const std::string &path)
        {
            bool ret = false;

            u::path cpath(path);
            std::map<std::string, std::vector<std::string> >::const_iterator it = storage.find(cpath.get("prefix"));

            if (it != storage.end()) {
                ret = std::find(it->first.begin(), it->first.end(), cpath.get(u::F | u::S));
            }
            return ret;
        }

        static bool ion_dir_check(const std::string &filename, ion_mode_e mode)
        {
            bool ret = true;
            std::string hint = u::format("warning: trying to load cache from [%s]\n."
                                         "         press 'ENTER' to continue.\n"
                                         "         press 'SPACE' to continue and add this folder to ignore file.\n"
                                         "         press '!' to continue and disable interactive mode for folders.\n"
                                         "         press 'ESC' to exit.\n", filename.c_str());
            int key = u::wait_key(0, hint);
            switch (key) {
            case u::key(u::KEY_ENTER):
                ret = true;
                break;
            case u::key(u::KEY_SPACE):
                ret = true;
                u::path path(filename);
                _ignores[mode].push_back(path.get("prefix"));
                break;
            case u::key("!"):
                ret = true;
                if (mode == ion_mode_e::ION_LOAD) {
                    ion_load_dirs(true);
                } else {
                    ion_save_dirs(true);
                }
                break;
            case u::key(u::KEY_ESC):
                ret = false;
                break;
            }

            return ret;
        }

        static bool ion_file_check(const std::string &filename, ion_mode_e mode)
        {
            std::string hint = u::format("warning: trying to load / save cache from / to [%s]\n."
                                         "         press 'ENTER' to continue.\n"
                                         "         press 'SPACE' to continue and add this file to ignore file.\n"
                                         "         press '!' to continue and disable interactive mode for files.\n"
                                         "         press 'ESC' to exit.\n", filename.c_str());
            int key = u::wait_key(0, hint);
            switch (key) {
            case u::key(u::KEY_ENTER):
                ret = true;
                u::path path(filename);
                _ignores[mode][path.get("prefix")].push_back(path.get(u::F | u::S));
                break;
            case u::key(u::KEY_SPACE):
                ret = true;
                if (mode == ion_mode_e::ION_LOAD) {
                    ion_load(true);
                } else {
                    ion_save(true);
                }
                break;
            case u::key(u::KEY_ESC):
                ret = false;
                break;
            }
            return ret;
        }

        /*
         *@function ion_check: check whether interactive mode or not
         *
         *@workflow
         *                                                                   --------------
         *                                                                   |file-bit = 1|
         *                                                                          |
         *                                                           ---------------------------------
         *                                                      true | +                       false | -
         *                                                     -------------                  --------------
         *                                                     |dir-bit = 1|                      ignore
         *                                                           |
         *                                 ----------------------------------------------------------
         *                            true | +                                                false | -
         *                         ------------------                                        ----------------
         *                         |dir in  _ignores|                                        |dir in _checks|
         *                                 |                                                        |
         *                   -----------------------------                            -----------------------------
         *              true | -                   false | +                     true | +                   false | -
         *          -----------------            ------------------           ------------------          -----------------
         *          |file in _checks|            |file in _ignores|           |file in _ignores|          |file in _checks|
         *                   |                           |                            |                           |
         *           ----------------             ----------------             ---------------             ---------------
         *      true | +      false | -      true | -      false | +      true | -     false | +      true | +     false | -
         *         check         ignore        ignore          check        ignore         check         check         ignore
         */
        inline static bool ion_check(const std::string &filename, ion_mode_e mode)
        {
            bool ret = true;
            if (ion(mode)) {
                ret = ion_dirs(mode);
                if (ret || ion_dir_check(filename, mode)) {
                    ret = true;
                    if (ion_include_dir(_ignores[mode], prefix)) {
                        if (ion_include_file(_checks[mode], file)) {
                            ret = ion_file_check();
                        }
                    } else {
                        if (! ion_include_file(_ignores[mode], file)) {
                            ret = ion_file_check();
                        }
                    }
                } else {
                    ret = true;
                    if (ion_include_dir(_checks[mode], prefix)) {
                        if (! ion_include_file(_ignores[mode], file)) {
                            ret = ion_file_check();
                        }
                    } else {
                        if (ion_include_file(_checks[mode], file)) {
                            ret = ion_file_check();
                        }
                    }
                }
            }

            return ret;
        }


        template <typename T, class data_reader=u::io::bin::default_data_reader<T> >
        static bool load(const std::string &filename, T &data, data_reader reader = data_reader())
        {
            bool ret = false;
            if (ion_check(filename, ion_mode_e::ION_LOAD)) {
                ret = reader(filename, data);
            }
            return ret;
        }

        template <typename T, class data_writer=u::io::bin::default_data_writer<T> >
        static bool save(const std::string &filename, T &data, data_writer writer = data_writer())
        {
            bool ret = false;
            if (ion_check(filename, ion_mode_e::ION_SAVE)) {
                ret = writer(filename, data);
            }
            return ret;
        }

    };

}

#endif
