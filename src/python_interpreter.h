#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <pybind11/pybind11.h>
#include <boost/filesystem.hpp>
#include <memory>

class PythonInterpreter {
public:
  PythonInterpreter();
  ~PythonInterpreter();
  void append_path(const boost::filesystem::path &path);
  bool import(const std::string &module_name);
  template <class... Args>
  pybind11::handle exec(const std::string &method_qualifier,
                        Args &&... args);
  pybind11::handle exec(const std::string &method_qualifier);
  void init();
private:
  std::unordered_map<std::string, pybind11::handle> modules;
};


#endif // JUCI_API_H_
