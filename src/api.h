#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <pybind11/pybind11.h>
#include <memory>

using namespace std;;
#include <iostream>

class PythonInterpreter {
public:
  PythonInterpreter();
  ~PythonInterpreter();
  void append_path(const std::wstring &path);
  bool import(const std::string &module_name);
  template <class... Args>
  pybind11::handle exec(const std::string &method_qualifier,
                        Args &&... args);
  void init();
private:
  std::unordered_map<std::string, pybind11::handle> modules;
};


#endif // JUCI_API_H_
