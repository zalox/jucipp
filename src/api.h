#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <unordered_map>
#include <string>
#include <Python.h>
#include <memory>
#include <pybind11/pybind11.h>

class PythonInterpreter {
public:
  PythonInterpreter();
  ~PythonInterpreter();
  pybind11::object exec(const std::string &module_name,
                        std::initializer_list<pybind11::object> args);
  pybind11::object import(const std::string &module_name);

private:
  std::unordered_map<std::string, pybind11::module> modules;
};


#endif // JUCI_API_H_