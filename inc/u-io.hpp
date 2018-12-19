#ifndef __U_IO_HPP__
#define __U_IO_HPP__

#include "u-version.hpp"

#include <fstream>
#include <vector>
#include <cassert>

#include "u-string.hpp"
#include "u-path.hpp"

namespace u {

  namespace io {

    template <typename T>
    class default_rec_writer
    {
      bool operator()(const T &data, std::ofstream &ofs) {
        ofs << data;
        return true;
      }
    };

    template <typename T, class data_writer = default_rec_writer<T> >
    static bool saverec(const std::string &filename, const std::vector<T> &data, data_writer writer=data_writer()) {
      bool ret = false;
      std::ofstream ofs(filename);
      if (!ofs.fail()) {
        for (size_t i=0; i<data.size(); ++i) {
          ret = writer(data[i], ofs);
          if (i+1 != data.size()) {
            ofs << std::endl;
          }
        }
      }
      ofs.close();
      return ret;
    }

    template <typename T>
    class default_rec_reader
    {
    public:
      bool operator()(T &data, const std::string &line) {
        assert(u::string::from_string<T>(line, data));
        return true;
      }
    };

    template <typename T, class data_reader=default_rec_reader<T> >
    static bool loadrec(const std::string &filename, std::vector<T> &storage, data_reader reader=data_reader()) {
      bool ret = false;
      storage.clear();
      int iline = 0;
      if (u::path::exists(filename.c_str(), u::F)) {
        std::ifstream ifs(filename.c_str());
        if (!ifs.fail()) {
          while (!ifs.eof()) {
            std::string line;
            std::getline(ifs, line);
            if (!line.empty()) {
              char *noret_line = u::string::trim(line, '\r', u::S);
              if (noret_line != NULL) {
                T data;
                if(reader(data, noret_line)) {
                  storage.push_back(data);
                }
              }
              u::string::free(&noret_line);
            }
            ++ iline;
          }
          ret = true;
        }
        ifs.close();
      }
      return ret;
    }

    template <typename T, class data_reader=default_rec_reader<T> >
    static std::vector<T> *loadrec(const std::string &filename, data_reader reader=data_reader()) {
      std::vector<T> *ret = new std::vector<T>();
      if (!loadrec<T, data_reader>(filename, *ret, reader)) {
        delete ret;
        ret = NULL;
      }
      return ret;
    }

    template <typename T>
    class default_data_writer
    {
    public:
      bool operator()(const std::vector<std::vector<T> > &data, std::ofstream &ofs, const std::string &sep) {
        for (size_t i = 0; i < data.size(); ++i) {
          for (size_t j = 0; j < data[i].size(); ++j) {
            ofs << data[i][j];
            if (j != data[i].size() - 1) {
              ofs << sep;
            }
          }
          ofs << std::endl;
        }
        return true;
      }
    };

    template <typename T, class data_writer = default_data_writer<T> >
    static bool savetxt(const std::string &filename, const std::vector<std::vector<T> > &data, data_writer writer=data_writer(), const std::string &sep = std::string(" ")) {
      bool ret = false;
      std::ofstream ofs(filename);
      if (!ofs.fail()) {
        ret = writer(data, ofs, sep);
      }
      ofs.close();
      return ret;
    }

    template <typename T>
    class default_data_reader
    {
    public:
      bool operator()(std::vector<T> &data, const std::vector<char *> &parts) {
        data.clear();
        for (size_t i = 0; i < parts.size(); ++i) {
          T _data;
          assert(u::string::from_string<T>(parts[i], _data));
          data.push_back(_data);
        }
        return true;
      }
    };

    template <typename T, class data_reader = default_data_reader<T> >
    static bool loadtxt(const std::string &filename, std::vector<std::vector<T> > &storage, data_reader reader=data_reader(),const std::string &sep = std::string(" ")) {
      bool ret = false;
      storage.clear();
      int iline = 0;
      if (u::path::exists(filename.c_str(), u::F)) {
        std::ifstream ifs(filename.c_str());
        if (!ifs.fail()) {
          while (!ifs.eof()) {
            std::string line;
            std::getline(ifs, line);
            if (!line.empty()) {
              std::vector<char *> parts = u::string::split(line, sep);
              std::vector<T> data;
              if(reader(data, parts)) {
              storage.push_back(data);
            }
            u::string::free(parts);
          }
          ++ iline;
        }
        ret = true;
      }
      ifs.close();
    }
    return ret;
  }

  template <typename T, class data_reader = default_data_reader<T> >
  static std::vector<std::vector<T> > *loadtxt(const std::string &filename, data_reader reader=data_reader(), const std::string &sep = std::string(" ")) {
    std::vector<std::vector<T> > *ret = new std::vector<std::vector<T> >();
    if (!loadtxt<T, data_reader>(filename, *ret, reader, sep)) {
      delete ret;
      ret = NULL;
    }
    return ret;
  }

  namespace bin {

    template <typename T>
    class default_bin_data_writer
    {
    public:
      bool operator()(std::ofstream &ofs, const T *data, int size) {
        assert(size > 0);
        bool ret = true;
        ofs.write(reinterpret_cast<const char*>(data), sizeof(T)*size);
        return ret;
      }
    };

    /*
     *@function write: write binary to file @filename
     *@params
     *  @filename file to be write
     *  @size size to be write, must be positive
     *  @reader read function/class used to write data
     *@return true if successfully, otherwise false
    **/
    template <typename T, class bin_data_writer = default_bin_data_writer<T> >
    static bool save(const std::string &filename, const T* data, int size, bin_data_writer writer=bin_data_writer()) {
      bool ret = false;
      if (u::path::exists(filename.c_str(), u::F)) {
        std::ofstream ofs(filename.c_str(), std::ofstream::out | std::ofstream::binary);
        if (!ofs.fail()) {
          ret = writer(ofs, data, size);
        }
        ofs.close();
      }
      return ret;
    }

    template <typename T, class bin_data_writer = default_bin_data_writer<T> >
    static bool save(const std::string &filename, const std::vector<T> data, bin_data_writer writer=bin_data_writer()) {
      return save<T, bin_data_writer>(filename, data.data(), data.size(), writer);
    }

    template <typename T>
    class default_bin_data_reader
    {
    public:
      bool operator()(std::ifstream &ifs, T *data, int size) {
        assert(size > 0);
        bool ret = true;
        ifs.read(reinterpret_cast<char*>(data), sizeof(T)*size);
        return ret;
      }
    };

    template <typename T, class bin_data_reader = default_bin_data_reader<T> >
    static int load(const std::string &filename, T* &data, int size, bin_data_reader reader=bin_data_reader()) {
      int ret = 0;
      if (u::path::exists(filename.c_str(), u::F)) {
        std::ifstream ifs(filename.c_str(), std::ifstream::in | std::ifstream::binary);
        if (!ifs.fail()) {
          if (size > 0) {
            data = new T[size];
            assert(reader(ifs, reinterpret_cast<char*>(data), sizeof(T)*size));
            ret = size;
          } else {
            std::vector<T> vdata;
            while (!ifs.eof()) {
              T rdata;
              assert(reader(ifs, &rdata, sizeof(T)));
              vdata.push_back(rdata);
            }
            ret = vdata.size();
            data = new T[ret];
            std::copy(vdata.begin(), vdata.end(), data);
          }
        }
        ifs.close();
      }
      return ret;
    }

    /*
     *@function load: load binary from file @filename
     *@params
     *  @filename file to be read
     *  @size size to be read, if @size is non-positive, read untill EOF and then set @size to read size
     *  @reader read function/class used to read data
     *@return array read data
    **/
    template <typename T, class bin_data_reader = default_bin_data_reader<T> >
    static T *load(const std::string &filename, int &size = -1, bin_data_reader reader=bin_data_reader()) {
      T * ret = NULL;
      int localsize = load<T, bin_data_reader>(filename, ret, size, reader);
      if (localsize <= 0) {
        delete ret;
        ret = NULL;
      } else { // if read successfully, override the correctly read size
        size = localsize;
      }
      return ret;
    }
  }
}

}

#endif
