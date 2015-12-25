#include "source_python.h"
#include "python_interpreter.h"
#include "config.h"

using namespace std;
#include <iostream>

Source::PythonView::PythonView(const boost::filesystem::path &file_path,
                               const boost::filesystem::path &project_path,
                               Glib::RefPtr<Gsv::Language> language) : GenericView(file_path, project_path, language) {
  parse_initialize();
}

void Source::PythonView::parse_initialize() {
  
  // is this python file a plugin?
  if (file_path.parent_path() != Config::get().juci_home_path()/"plugins") {
    return;
  }
  
  auto &modules = PythonInterpreter::get().modules;
  auto module_name = file_path.filename().string();
  auto module = modules.find(module_name);
  if (module == modules.end()) {
    return;
  }
  // hack this by mocking libjuci
  pybind11::handle ptr = PyImport_ReloadModule(module->second.ptr());
  std::string error_msg, error;
  int line_number, offset;
  if (PythonInterpreter::get().parse_syntax_error(error_msg, error, line_number, offset)) {
    
  } else if (!PyErr_Occurred()) {
    modules[module_name] = ptr;
    return;
  }
  ptr.dec_ref(); 
}