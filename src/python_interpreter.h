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
  bool import(const std::string &module_name);
  template <class... Args>
  pybind11::handle exec(const std::string &method_qualifier,
                        Args &&... args);
  pybind11::handle exec(const std::string &method_qualifier);
  bool parse_syntax_error(std::string &error_msgs, std::string &error, int &line_number, int &offset);
  void handle_syntax_error();
  std::unordered_map<std::string, pybind11::handle> modules;
  private:
  PythonInterpreter();
};


#endif // JUCI_API_H_
