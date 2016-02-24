#include "python_interpreter.h"
#include "python_api.h"
#include "config.h"
#include "terminal.h"
#include "juci.h"

PythonInterpreter& PythonInterpreter::get(){
  static PythonInterpreter s;
  return s;
}

PythonInterpreter::PythonInterpreter(){
#ifdef _WIN32
  auto root_path=Config::get().terminal.msys2_mingw_path;
  append_path(root_path/"include/python3.5m");
  append_path(root_path/"lib/python3.5");
  long long unsigned size = 0L;
#else
  long unsigned size = 0L;
#endif
  auto plugin_path=Config::get().juci_home_path()/"plugins";
  if(!boost::filesystem::exists(plugin_path))
    Config::get().load();
  append_path(plugin_path);
  PyImport_AppendInittab("libjuci",init_juci_api);
  Py_Initialize();
  argv=Py_DecodeLocale("",&size);
  PySys_SetArgv(0,&argv);
  boost::filesystem::directory_iterator end_it;
  for(boost::filesystem::directory_iterator it(plugin_path);it!=end_it;it++){
    auto module_name = it->path().stem().generic_string();
    import(module_name);
  }
}

PythonInterpreter::~PythonInterpreter(){
  for(auto &module:modules)
    module.second.dec_ref();
  handle_py_exception();
  if(Py_IsInitialized())
    Py_Finalize();
}

void PythonInterpreter::append_path(const boost::filesystem::path &path){
  std::wstring res(Py_GetPath());
  if(!res.empty())
#ifdef _WIN32
    res += ';';
#else
    res += ':';
#endif
  res += path.generic_wstring();
  Py_SetPath(res.c_str());
}

bool PythonInterpreter::import(const std::string &module_name){
  pybind11::str str(module_name.c_str());
  auto module = modules.find(module_name);
  if(module == modules.end()){
    pybind11::handle new_module(PyImport_ImportModule(module_name.c_str()));
    if(new_module){
      modules[module_name] = new_module;
      return true;
    }
  }else{
    pybind11::handle reload_module(PyImport_ReloadModule(module->second.ptr()));
    if(reload_module){
      module->second.dec_ref();
      modules[module_name] = reload_module;
      return true;
    }else
      reload_module.dec_ref();
  }
  handle_py_exception();
  return false;
}

pybind11::handle PythonInterpreter::exec(const std::string &method_qualifier){
  auto pos = method_qualifier.rfind('.');
  if (pos == std::string::npos)
    return nullptr;
  auto module_name=method_qualifier.substr(0,pos);
  auto method=method_qualifier.substr(module_name.length()+1,method_qualifier.size());
  auto module=modules.find(module_name);
  if (module == modules.end())
    return nullptr;
  pybind11::handle func(module->second.attr(method.c_str()));
  if(func && PyCallable_Check(func.ptr()))
    try{
      return func.call();
    }catch(const std::exception &ex){

    }
  handle_py_exception();
  return nullptr;
}

template <class... Args>
pybind11::handle PythonInterpreter::exec(const std::string &method_qualifier,
                                         Args &&... args){
  auto pos=method_qualifier.rfind('.');
  if(pos == std::string::npos)
    return nullptr;
  auto module_name=method_qualifier.substr(0,pos);
  auto method=method_qualifier.substr(module_name.length()+1,method_qualifier.size());
  auto module=modules.find(module_name);
  if(module == modules.end())
    return nullptr;

  auto func=pybind11::handle(module->second.attr(method.c_str()));
  if(func && PyCallable_Check(func.ptr()))
    return func.call(std::forward<Args>(args)...);
  return nullptr;
}

void PythonInterpreter::handle_py_exception(){
  pybind11::handle error(PyErr_Occurred());
  if(error){
    PyObject *exception,*value,*traceback;
    PyErr_Fetch(&exception,&value,&traceback);
    PyErr_NormalizeException(&exception,&value,&traceback);
    pybind11::object py_exception(exception,false);
    pybind11::object py_value(value,false);
    pybind11::object py_traceback(traceback,false);
    std::stringstream str;
    if(PyErr_GivenExceptionMatches(py_exception.ptr(),PyExc_SyntaxError)){
      std::string error_msgs,error;
      int line_number=0,offset=0;
      if(parse_syntax_error(py_value,error_msgs,error,line_number,offset))
        str << error_msgs << " (" << line_number << ":" << offset << "):\n" << error;
      else
        str << "An error occured while trying to parse SyntaxError\n";
    }
    else if(PyErr_GivenExceptionMatches(py_exception.ptr(),PyExc_AttributeError))
      str << "AttributeError: " << py_value.str().operator const char *() << "\n";
    else if(PyErr_GivenExceptionMatches(py_exception.ptr(),PyExc_ImportError))
      str << "ImportError: " << py_value.str().operator const char *() << "\n";
    else
      str << py_exception.str().operator const char*() << "\n" << py_value.str().operator const char*() << "\n";
    if(Terminal::get().is_visible())
      Terminal::get().print(str.str());
    else
      std::cerr << str.rdbuf();
  }
}

bool PythonInterpreter::parse_syntax_error(pybind11::object &py_value,std::string &error_msg,std::string &error,int &line_number,int &offset){
  _Py_IDENTIFIER(msg); // declares PyID_msg
  _Py_IDENTIFIER(lineno);
  _Py_IDENTIFIER(offset);
  _Py_IDENTIFIER(text);
  pybind11::str py_error_msg(_PyObject_GetAttrId(py_value.ptr(),&PyId_msg),false);
  pybind11::str py_error_text(_PyObject_GetAttrId(py_value.ptr(),&PyId_text),false);
  pybind11::object py_line_number(_PyObject_GetAttrId(py_value.ptr(),&PyId_lineno),false);
  pybind11::object py_line_offset(_PyObject_GetAttrId(py_value.ptr(),&PyId_offset),false);
  if(py_line_number.ptr()!=Py_None && py_line_offset.ptr()!=Py_None && py_error_msg.ptr()!=Py_None && py_error_text.ptr()!=Py_None){
    line_number=PyLong_AsLong(py_line_number.ptr()); // TODO these pymethod can produce pyerrors
    offset=PyLong_AsLong(py_line_offset.ptr());
    error_msg=std::string(py_error_msg);
    error=std::string(py_error_text);
    return true;
  }
  error_msg="";
  error="";
  line_number=0;
  offset=0;
  return false;
}
