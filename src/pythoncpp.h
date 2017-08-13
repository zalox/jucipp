#ifndef JUCI_PYTHON_INTERPRETER_H_
#define JUCI_PYTHON_INTERPRETER_H_

#ifdef _WIN32
#include <cmath>
#define _hypot hypot
#endif

#include <pybind11/pybind11.h>

namespace py {
  class err {
  public:
    pybind11::object static occurred();
  };

  class import {
  public:
    pybind11::module static add_module(const std::string &module_name);
    pybind11::module static reload(pybind11::module &module);
    pybind11::dict static get_module_dict();
  };

  class Interpreter {
  public:
    Interpreter();
    ~Interpreter();
  };
};

#endif // JUCI_PYTHON_INTERPRETER_H_

// const std::vector<boost::filesystem::path> python_path = {
//   Config::get().python.path,
//   Config::get().python.plugin_path
// };
// std::wstring sys_path;
// for(auto &path:python_path){
//   std::wcout << sys_path << std::endl;
//   if(path.empty())
//     continue;
//   if(!sys_path.empty()){
//   #ifdef _WIN32
//       sys_path += ';';
//   #else
//       sys_path += ':';
//   #endif
//   }
//   sys_path += path.generic_wstring();
// }
// Py_SetPath(sys_path.c_str());

// #ifdef _WIN32
//   long long unsigned size = 0L;
// #else
// // long unsigned size = 0L;
// #endif
