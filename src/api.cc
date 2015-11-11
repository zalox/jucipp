#include "api.h"
#include "singletons.h"

void PythonApi::directories_open(const std::string &dir) {
  Singleton::directories->open(dir);
}

PythonInterpreter::PythonInterpreter() {
  try {
    if (!Py_IsInitialized()) {
      Py_Initialize();
      auto main = boost::python::import("__main__");
      name = main.attr("__dict__");
    }
  } catch (boost::python::error_already_set &error) {
    PyErr_Print();
  }
}
PythonInterpreter::~PythonInterpreter() {
  if(Py_IsInitialized()){
    Py_Finalize();
  }
}
void PythonInterpreter::python_exec(const std::string &command) {
  try {
    boost::python::exec(command.c_str(), name);
  } catch (boost::python::error_already_set &error) {
    PyErr_Print();
  }
}
void PythonInterpreter::init() {
  auto plugin_path = Singleton::config->juci_home_path() / "plugins";
  python_exec("import sys");
  python_exec("sys.path.append('" + plugin_path.string() + "')");
  python_exec("sys.path.append('/home/zalox/projects/juci/src')");
  python_exec("import jucipy");
  if (boost::filesystem::exists(plugin_path) && boost::filesystem::is_directory(plugin_path)) {
    for (boost::filesystem::directory_iterator it(plugin_path); it != boost::filesystem::directory_iterator(); it++) {
      auto import = it->path().filename();
      while (!import.extension().empty()) { // make sure extensions are stripped on files
        import = import.stem();
      }
      python_exec("import " + import.string());
    }
  }
}
