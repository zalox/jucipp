#include "api.h"
#include <pybind11/pybind11.h>
#include "singletons.h"

PythonInterpreter::PythonInterpreter() {
  if (!Py_IsInitialized()) {
    Py_Initialize();
  }
  import("sys");
  exec("sys.path.append", {pybind11::str((Singleton::config->juci_home_path() / "plugins").c_str())});
}

PythonInterpreter::~PythonInterpreter() {
  if (Py_IsInitialized()) {
    Py_Finalize();
  }
}

pybind11::object PythonInterpreter::import(const std::string &module_name) {
  Singleton::terminal->print("importing " + module_name);
  auto module = pybind11::object(PyImport_ImportModule(module_name.c_str()), false);
  if (module.ptr() == nullptr) {
    Singleton::terminal->print("Failed to import " + module_name + "\n");
    PyErr_Print();
    return pybind11::object();
  }
  modules[module_name] = module;
  return module;
}

pybind11::object PythonInterpreter::exec(const std::string &methodstring,
                                         std::initializer_list<pybind11::object> args = {}) {
  Singleton::terminal->print("Executing: " + methodstring);
  auto module_name = methodstring.substr(0, methodstring.find('.'));
  auto method = methodstring.substr(module_name.length(), methodstring.size());
  auto module = modules.find(module_name);
  if (module == modules.end()) {
    Singleton::terminal->print("Method string not executed:" + methodstring);
  } else {
    auto func = pybind11::object(PyObject_GetAttrString(module->second.ptr(), method.c_str()), false);
    if (func.ptr() && PyCallable_Check(func.ptr())) {
      pybind11::tuple tuple(args.size());
      size_t i = 0;
      for (auto it = args.begin(); it != args.end(); it++) {
        pybind11::object arg = *it;
        tuple[i] = arg;
        i++;
      }
      Singleton::terminal->print("Executed: " + methodstring);
      return pybind11::object(PyObject_CallObject(func.ptr(), tuple.ptr()), false);
    }
  }
  return pybind11::object();
}
