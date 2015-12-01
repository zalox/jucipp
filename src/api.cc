#include "api.h"
#include "singletons.h"

PythonInterpreter::PythonInterpreter() {
  append_path(L"/home/zalox/projects/juci/src/");
  append_path((Singleton::config->juci_home_path()/"plugins").wstring());
  Py_Initialize();
}

void PythonInterpreter::init() {
  import("juciplugin");
  exec("juciplugin.open", "/home/");
}

PythonInterpreter::~PythonInterpreter() {
  for (auto &module : modules) {
    module.second.dec_ref();
  }
  if (PyErr_Occurred() != nullptr)
    PyErr_Print();
  Py_Finalize();
}

void PythonInterpreter::append_path(const std::wstring &path) {
  std::wstring res(path);
#ifdef _WIN32
  res += ';';
#else
  res += ':';
#endif
  res += Py_GetPath();
  Py_SetPath(res.c_str());
}

bool PythonInterpreter::import(const std::string &module_name) {
  pybind11::str str(module_name.c_str());
  auto module = pybind11::handle(PyImport_Import(str.ptr()));
  if (module) {
    module.inc_ref();
    modules[module_name] = module;
    return true;
  }
  PyErr_Print();
  return false;
}
template <class... Args>
pybind11::handle PythonInterpreter::exec(const std::string &method_qualifier,
					 Args &&... args) {
  auto pos = method_qualifier.rfind('.');
  if (pos == std::string::npos) {
    cerr << "Method <module>.<submodule (optional)>.<method>" << endl;
  }
  auto module_name = method_qualifier.substr(0, pos);
  auto method = method_qualifier.substr(module_name.length()+1, method_qualifier.size());
  auto module = modules.find(module_name);
  if (module == modules.end()) {
    cerr << module_name << " is not found, you should try to import it" << endl;
    return nullptr;
  }
  auto func = pybind11::handle(module->second.attr(method.c_str()));
  if (func && PyCallable_Check(func.ptr())) {
    return func.call(args...);
  }
  return nullptr;
}
