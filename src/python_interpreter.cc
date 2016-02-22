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
    Config::get().load();
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
    handle_syntax_error();
    handle_py_exception();
    return false;
  } else {
    pybind11::handle reload_module(PyImport_ReloadModule(module->second.ptr()));
    if(reload_module){
      module->second.dec_ref();
      modules[module_name] = reload_module;
      return true;
    } else {
      reload_module.dec_ref();
      handle_syntax_error();
      handle_py_exception();
      return false;
    }
  }
}

void PythonInterpreter::handle_syntax_error() {
  std::string error_msgs, error;
  int line_number=0, offset=0;
  if (parse_syntax_error(error_msgs, error, line_number, offset)) {
    std::stringstream str;
    str << "Error while reloading,\n"
    << error_msgs << " (" << line_number << ":" << offset << "):\n" << error;
    Terminal::get().print(str.str());
  }
}

void PythonInterpreter::handle_py_exception() {
  if(PyErr_Occurred()) {
    std::cerr << "An error occured during python execution, no information was gathered\n";
    PyErr_Clear();
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
  pybind11::handle func = module->second.attr(method.c_str());
  if (func && PyCallable_Check(func.ptr())) {
    try {
      return func.call();
    } catch (std::exception &ex) {
      handle_syntax_error();
      handle_py_exception();
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
      PyErr_Clear();
      return true;
    }
  }
  return false;
}