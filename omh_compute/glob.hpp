#ifndef __GLOB_H__
#define __GLOB_H__

#include <cstring>
#include <stdexcept>
#include <glob.h>
#include <vector>


class glob_files {
protected:
  static std::string strerror_r(int ret, const std::vector<char>& s) {
    return std::string(ret == 0 ? s.data() : "Unknown error");
  }
  static std::string strerror_r(char* ret, const std::vector<char>& s) {
    return std::string(ret);
  }
  static std::string strerror_r(int errnum) {
    std::vector<char> res(1024, '\0');
    return strerror_r(::strerror_r(errnum, res.data(), res.size()-1), res);
  }

  static int errfunc(const char* epath, int eerrno) {
    std::cerr << "Glob warning: " << strerror_r(eerrno) << '\n';
    return 0;
  }

  glob_t pglob;
public:
  glob_files(const char* pattern) {
    const int ret = ::glob(pattern, GLOB_NOSORT, errfunc, &pglob);
    if(ret != 0 && ret != GLOB_NOMATCH)
      throw std::runtime_error(std::string("Glob error"));
  }

  ~glob_files() {
    globfree(&pglob);
  }

  char** const begin() const { return pglob.gl_pathv; }
  char** const end() const { return begin() + pglob.gl_pathc; }

  size_t size() const { return pglob.gl_pathc; }
};

#endif /* __GLOB_H__ */
