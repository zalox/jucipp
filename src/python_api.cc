#include "api.h"
#include <pybind11/pybind11.h>

PYBIND11_PLUGIN(libjuci) {
  pybind11::module m("libjuci", "Python API for juCi++");
  pybind11::class_<PythonApi>(m, "API")
    .def(pybind11::init())
    .def("directories_open", &PythonApi::directories_open)
  ;
  return m.ptr();
}