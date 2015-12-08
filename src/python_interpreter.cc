#include "python_interpreter.h"
#include "python_api.h"
#include "singletons.h"
#include "juci.h"

PythonInterpreter::PythonInterpreter() {}

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

using namespace std;
#include <iostream>

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
      modules[module_name] = reload_module;
      module->second.dec_ref();
      return true;
    } else {
      PyErr_Print();
      reload_module.dec_ref();
      Singleton::terminal->print("Error while reloading plugin "+module_name+", check syntax");
      return false;
    }
  }
}

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
  auto func = pybind11::handle(module->second.attr(method.c_str()));
  if (func && PyCallable_Check(func.ptr())) {
    auto obj = func.call();
    func.dec_ref();
    return obj;
  }
  func.dec_ref();
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
    auto obj = func.call(std::forward<Args>(args)...);
    func.dec_ref();
    return obj;
  }
  func.dec_ref();
  return nullptr;
}
