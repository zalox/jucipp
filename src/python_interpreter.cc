#include "python_interpreter.h"
#include "python_api.h"
#include "config.h"
#include "terminal.h"
#include "juci.h"

PythonInterpreter& PythonInterpreter::get() {
  static PythonInterpreter s;
  return s;
}

PythonInterpreter::PythonInterpreter() {
#ifdef _WIN32
  auto root_path = Config::get()->juci_home_path();
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
  auto plugin_path = Config::get().juci_home_path() / "plugins";
  if(!boost::filesystem::exists(plugin_path))
    Config::get();
  append_path(plugin_path);
  PyImport_AppendInittab("libjuci", init_juci_api);
  Py_Initialize();
  boost::filesystem::directory_iterator end_it;
  for(boost::filesystem::directory_iterator it(plugin_path);it!=end_it;it++) {
    auto module_name = it->path().stem().string();
    import(module_name);
  }
}

std::string PythonInterpreter::generate_mock(pybind11::dict &dict, const std::string &indent)  {
  std::string name(pybind11::str(dict["__name__"], false));
  std::string class_name;
  auto pos = name.rfind('.');
  if (pos != std::string::npos)
    class_name = name.substr(pos+1, name.size());
  else
    class_name = name;
  
  std::string res = indent + "class " + class_name + ":\n";
  for (const auto item : dict) {
    std::string key(item.first.str()), value(item.second.str());
    if(key.substr(0, 2) == "__") {
        continue;
    } else if (value.substr(0, 2) == "<m") {
      pybind11::module submodule(PyImport_AddModule(std::string(name+"."+key).c_str()), false);
      pybind11::dict submoduledict(PyModule_GetDict(submodule.ptr()), false);
      res += generate_mock(submoduledict, "  ");
    } else {
      res += indent + "  " + key + "():\n";
    }
  }
  return res;
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
    Terminal::get().print("Error while loading plugin " + module_name + ", check syntax\n");
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
      Terminal::get().print("Error while reloading plugin " + module_name + ", check syntax\n");
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
    try {
      return func.call();
    } catch (std::exception &ex) {
      if (PyErr_Occurred() != nullptr)
        PyErr_Print();
      std::string str("Error:\n");
      str += ex.what();
      str += "\n";
      Terminal::get().print(str);
      return nullptr;
    }
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


bool PythonInterpreter::parse_syntax_error(std::string &error_msg, std::string &error, int &line_number, int &offset) {
  pybind11::handle obj;
  if ((obj = PyErr_Occurred()) && PyErr_GivenExceptionMatches(obj.ptr(), PyExc_SyntaxError)) {
    _Py_IDENTIFIER(msg); // declares PyID_msg
    _Py_IDENTIFIER(lineno);
    _Py_IDENTIFIER(offset);
    _Py_IDENTIFIER(text);
    /**
     * text should contain the error
     */
    PyObject *exception, *value, *traceback;
    PyErr_Fetch(&exception, &value, &traceback);
    PyErr_NormalizeException(&exception, &value, &traceback);
    pybind11::object py_exception(exception, false);
    pybind11::object py_value(value, false);
    pybind11::object py_traceback(traceback, false);
    PyErr_Restore(exception, value, traceback); // "catches" the exception
    pybind11::str py_error_msg(_PyObject_GetAttrId(py_value.ptr(), &PyId_msg), false);
    pybind11::str py_error_text(_PyObject_GetAttrId(py_value.ptr(), &PyId_text), false);
    pybind11::object py_line_number(_PyObject_GetAttrId(py_value.ptr(), &PyId_lineno), false);
    pybind11::object py_line_offset(_PyObject_GetAttrId(py_value.ptr(), &PyId_offset), false);
    if (py_line_number.ptr() != Py_None &&
        py_line_offset.ptr() != Py_None &&
        py_error_msg.ptr() != Py_None &&
        py_error_text.ptr() != Py_None)
    {
      line_number = PyLong_AsLong(py_line_number.ptr()); // TODO these pymethod can produce pyerrors
      offset = PyLong_AsLong(py_line_offset.ptr());
      error_msg = std::string(py_error_msg);
      error = std::string(py_error_text);
      return true;
    }
  }
  return false;
}