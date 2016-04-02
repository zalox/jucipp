#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <pybind11/pybind11.h>
#include <boost/filesystem.hpp>
#include <memory>

class PythonInterpreter {
public:
  ~PythonInterpreter();
  static PythonInterpreter& get();
  void init();
  void append_path(const boost::filesystem::path &path);
  pybind11::object exec(const std::string &method_qualifier);
  bool reload(const std::string &module_name);
  bool parse_syntax_error(pybind11::object &py_value,std::string &error_msgs,std::string &error,int &line_number,int &offset);
  void handle_py_exception(bool suppress_error_messages=false);
  pybind11::module import(const std::string &module_name);
private:
  PythonInterpreter();
  wchar_t *argv;
};


#endif // JUCI_API_H_
