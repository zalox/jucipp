#include "api.h"
#include <pybind11/pybind11.h>
#include "singletons.h"

PYBIND_PLUGIN(libjuci) {
  pybind11::module m("libjuci", "Python API for juCi++");
  m.def("directories_open", [](const std::string &dir) {
    boost::filesystem::path path(dir);
    if (boost::filesystem::is_directory(path))
      Singleton::directories->open(path);
  }, "Opens directory in file tree", pybind11::arg("String representation of the path"));
  return m.ptr();
}