#ifndef JUCI_API_H_
#define JUCI_API_H_

#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <gtkmm.h>
#include <boost/python.hpp>
#include "singletons.h"

class PythonApi {
public:
  static void directories_open(const std::string &dir);
};

class PythonInterpreter {
public:
  PythonInterpreter();
  ~PythonInterpreter();
  void init();
private:
  boost::python::object name;
  void python_exec(const std::string &command);
};


#endif // JUCI_API_H_