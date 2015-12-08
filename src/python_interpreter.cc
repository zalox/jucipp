#include "python_interpreter.h"
#include "python_api.h"
#include "singletons.h"
#include "juci.h"

PythonInterpreter::PythonInterpreter() {
#ifdef _WIN32
  auto root_path = Singleton::config->juci_home_path();
  for(size_t i = 0; i < 3; i++) {
    root_path = root_path.parent_path();
  }
  auto p = root_path/"mingw64/lib/python3.5";
  Py_SetPath(p.generic_wstring().c_str());
  append_path(root_path/"mingw64/include/python3.5m");
  Py_Initialize();
  if(PyErr_Occurred() != nullptr) {
    auto pp = root_path/"mingw32/lib/python3.5";
    Py_SetPath(pp.generic_wstring().c_str());
    append_path(root_path/"mingw32/include/python3.5m");
    Py_Initialize();
    if(PyErr_Occurred() == nullptr) {
      throw std::runtime_error("Couldn't find python libraries");
    }
  }
  PyErr_Clear();
  Py_Finalize();
#endif
}

void PythonInterpreter::init() {
  auto plugin_path = Singleton::config->juci_home_path() / "plugins";
  append_path(plugin_path);
  PyImport_AppendInittab("libjuci", init_juci_api);
  Py_Initialize();
  boost::filesystem::directory_iterator end_it;
  for(boost::filesystem::directory_iterator it(plugin_path);it!=end_it;it++) {
    auto module_name = it->path().stem().string();
    import(module_name);
  }
}

PythonInterpreter::~PythonInterpreter() {
  for (auto &module : modules) {
    module.second.dec_ref();
  }
  if (PyErr_Occurred() != nullptr)
    PyErr_Print();
  if (Py_IsInitialized())
    Py_Finalize();
}

void PythonInterpreter::append_path(const boost::filesystem::path &path) {
  std::wstring res(Py_GetPath());
  if(!res.empty()) {
#ifdef _WIN32
  res += ';';
#else
  res += ':';
#endif
  }
  res += path.generic_wstring();
  Py_SetPath(res.c_str());
}

bool PythonInterpreter::import(const std::string &module_name) {
  pybind11::str str(module_name.c_str());
  auto module = modules.find(module_name);
  if(module == modules.end()) {
    pybind11::handle new_module(PyImport_ImportModule(module_name.c_str()));
    if (new_module) {
      modules[module_name] = new_module;
      return true;
    }
    PyErr_Print();
    Singleton::terminal->print("Error while loading plugin "+module_name+", check syntax");
    return false;
  } else {
    pybind11::handle reload_module(PyImport_ReloadModule(module->second.ptr()));
    if(reload_module){
      module->second.dec_ref();
      modules[module_name] = reload_module;
      return true;
    } else {
      PyErr_Print();
      reload_module.dec_ref();
      //TODO print syntax errors or add linter to Source::View
      Singleton::terminal->print("Error while reloading plugin "+module_name+", check syntax");
      return false;
    }
  }
}

using namespace std;
#include <iostream>

pybind11::handle PythonInterpreter::exec(const std::string &method_qualifier){
  auto pos = method_qualifier.rfind('.');
  if (pos == std::string::npos) {
    return nullptr;
  }
  auto module_name = method_qualifier.substr(0, pos);
  auto method = method_qualifier.substr(module_name.length() + 1, method_qualifier.size());
  auto module = modules.find(module_name);
  if (module == modules.end()) {
    return nullptr;
  }
  pybind11::handle func = module->second.attr(method.c_str());
  if (func && PyCallable_Check(func.ptr())) {
    return func.call();
  }
  return nullptr;
}

template <class... Args>
pybind11::handle PythonInterpreter::exec(const std::string &method_qualifier,
                                         Args &&... args) {
  auto pos = method_qualifier.rfind('.');
  if (pos == std::string::npos) {
    return nullptr;
  }
  auto module_name = method_qualifier.substr(0, pos);
  auto method = method_qualifier.substr(module_name.length() + 1, method_qualifier.size());
  auto module = modules.find(module_name);
  if (module == modules.end()) {
    return nullptr;
  }
  auto func = pybind11::handle(module->second.attr(method.c_str()));
  if (func && PyCallable_Check(func.ptr())) {
    return func.call(std::forward<Args>(args)...);
  }
  return nullptr;
}
