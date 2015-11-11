#include "api.h"
#include <pybind11/pybind11.h>

PYBIND11_PLUGIN(jucipy) {
  pybind11::module m("jucipy", "Python API for juCi++");
  pybind11::class_<PythonApi>(m, "API")
    .def(pybind11::init<>())
    .def("directories_open", &PythonApi::directories_open, pybind11::arg("dir"))
  ;
  return m.ptr();
}