#include "python_interpreter.h"
#include "python_api.h"
#include "config.h"
#include "terminal.h"
#include "juci.h"

PythonInterpreter& PythonInterpreter::get(){
  static PythonInterpreter s;
  return s;
}

bool PythonInterpreter::reload(const std::string &module_name){
  pybind11::module module(PyImport_AddModule(module_name.c_str()), true);
  if(module){
    pybind11::module reload(PyImport_ReloadModule(module.ptr()), false);
    if(reload){
      return true;
    }
  }
  handle_py_exception();
  return false;
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
    if(module_name != "__pycache__")
      try{
        pybind11::module::import(module_name.c_str());
      } catch (const std::exception &ex) { }
  }
}

PythonInterpreter::~PythonInterpreter(){
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

pybind11::object PythonInterpreter::exec(const std::string &method_qualifier){
  auto pos = method_qualifier.rfind('.');
  if (pos == std::string::npos)
    return pybind11::object(nullptr, false);
  auto module_name=method_qualifier.substr(0,pos);
  auto method=method_qualifier.substr(module_name.length()+1,method_qualifier.size());
  auto module=pybind11::module(PyImport_AddModule(module_name.c_str()), true);
  if(module){
    pybind11::handle func(module.attr(method.c_str()));
    if(func && PyCallable_Check(func.ptr())){
      try{
        return func.call();
      }catch(const std::exception &ex){
        Terminal::get().print(std::string(ex.what()) + "\n");
      }
    }
  }
  handle_py_exception();
  return pybind11::object(nullptr, false);
}

void PythonInterpreter::handle_py_exception(bool suppress_error_messages){
  pybind11::handle error(PyErr_Occurred());
  if(error){
    PyObject *exception,*value,*traceback;
    PyErr_Fetch(&exception,&value,&traceback);
    PyErr_NormalizeException(&exception,&value,&traceback);
    pybind11::object py_exception(exception,false);
    pybind11::object py_value(value,false);
    pybind11::object py_traceback(traceback,false);
    if(suppress_error_messages){
      return;
    }
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
      str << "AttributeError: " << std::string(py_value.str()) << "\n";
    else if(PyErr_GivenExceptionMatches(py_exception.ptr(),PyExc_ImportError))
      str << "ImportError: " << std::string(py_value.str()) << "\n";
    else
      str << std::string(py_exception.str()) << "\n" << std::string(py_value.str()) << "\n";
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
